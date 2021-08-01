#include "projects_generator.hpp"

#include <GL/glew.h>  // Initialize with glewInit()
#include <GLFW/glfw3.h>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <inja/inja.hpp>
#include <utils.hpp>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"

ProjectsGenerator::ProjectsGenerator(std::string templates_path)
    : file_browser(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir),
      templates_path_(std::move(templates_path)) {
  const auto config_path = templates_path_ / "config.yaml";
  if (std::filesystem::exists(config_path)) {
    templates = YAML::LoadFile(config_path)["templates"].as<std::vector<Template>>();
  } else {
    spdlog::error("Path {} doesn't exists.", config_path.string());
  }
  spdlog::cfg::load_env_levels();
  // Setup window
  glfwSetErrorCallback(
      [](int error, const char *description) { spdlog::error("Glfw error {}: {}\n", error, description); });
  if (glfwInit() == 0) {
    exit(EXIT_FAILURE);
  }

  // Decide GL+GLSL versions
  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  // Create window with graphics context
  window_ = glfwCreateWindow(1280, 720, "ImGui Demo", nullptr, nullptr);

  if (window_ == nullptr) {
    exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(window_);
  glfwSwapInterval(1);  // Enable vsync

  // Initialize OpenGL loader
  if (glewInit() != GLEW_OK) {
    spdlog::error("Failed to initialize OpenGL loader!");
    exit(EXIT_FAILURE);
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

ProjectsGenerator::~ProjectsGenerator() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window_);
  glfwTerminate();
}

void ProjectsGenerator::run() {  // Main loop
  while (glfwWindowShouldClose(window_) == 0) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to
    // tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
    // your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input
    // data to your main application. Generally you may always pass all inputs
    // to dear imgui, and hide them from your application based on those two
    // flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Main application
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    if (ImGui::Begin("Example: Fullscreen window",
                     nullptr,
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration |
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar)) {
      if (ImGui::BeginCombo("Templates", templates[current_template].name.c_str())) {
        for (std::size_t i = 0; i < templates.size(); ++i) {
          const bool is_selected = (current_template == i);
          if (ImGui::Selectable(templates[i].name.c_str(), is_selected)) {
            current_template = i;
          }
          if (is_selected) {
            ImGui::SetItemDefaultFocus();
          }
        }
        ImGui::EndChild();
      }
      // 2 columns: name & value
      if (ImGui::BeginTable("Variables", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable)) {
        for (auto &variable : templates[current_template].variables) {
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("%s %s", variable.name.c_str(), variable.optional ? "(optional)" : "");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", variable.description.c_str());
          }
          ImGui::TableNextColumn();
          ImGui::SetNextItemWidth(-1);
          const auto variable_value = fmt::format("##{}_value", variable.name);
          ImGui::InputText(variable_value.c_str(), &variable.value);
        }
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        if (ImGui::Button("Select path")) {
          file_browser.Open();
        }
        file_browser.Display();
        if (file_browser.HasSelected()) {
          package_path = file_browser.GetSelected().string();
          file_browser.ClearSelected();
        }
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("package_path", &package_path, ImGuiInputTextFlags_CharsNoBlank);
        ImGui::EndTable();
      }
      if (ImGui::Button("Generate")) {
        if (std::any_of(
                templates[current_template].variables.cbegin(),
                templates[current_template].variables.cend(),
                [](const TemplateVariable &variable) { return variable.value.empty() && !variable.optional; })) {
          ImGui::OpenPopup("Missing value");
          error_message = "Missing one of the template parameters";
        } else if (!std::filesystem::exists(package_path)) {
          error_message = "The path specified doesn't exists";
          ImGui::OpenPopup("Missing value");
        } else {
          inja::Environment env;
          env.set_line_statement("{##}");
          env.add_callback("camelCaseToSnakeCase", 1, [](inja::Arguments args) {
            return camelCaseToSnakeCase(args[0]->get<std::string>());
          });
          env.add_callback("underscoreToDash", 1, [](inja::Arguments args) {
            return underscoreToDash(args[0]->get<std::string>());
          });
          env.add_callback("capitalizeWordsWithWhitespace", 1, [](inja::Arguments args) {
            return capitalizeWordsWithWhitespace(args[0]->get<std::string>());
          });
          env.add_callback("getCurrentYear", 0, [](const inja::Arguments & /* args */) { return getCurrentYear(); });
          nlohmann::json data;
          for (auto &variable : templates[current_template].variables) {
            data[variable.name] = variable.value;
          }
          for (const auto &file : templates[current_template].files) {
            const auto template_path = templates_path_ / templates[current_template].name / file.name;
            const auto output_path = std::filesystem::path(package_path) / env.render(file.output_path, data);
            spdlog::info("Generating `{}` from `{}`", output_path.string(), template_path.string());
            std::filesystem::create_directories(output_path.parent_path());
            const auto parsed_template = env.parse_template(template_path);
            env.write(parsed_template, data, output_path);
          }
        }
      }
      ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
      if (ImGui::BeginPopupModal("Missing value", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("%s", error_message.c_str());
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) {
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }
    }
    ImGui::End();
    // Rendering
    ImGui::Render();
    int display_w = 0;
    int display_h = 0;
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.45F, 0.55F, 0.60F, 1.00F);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window_);
  }
}
