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
    : template_variables_value(template_variables.size()),
      file_browser(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir),
      templates_path_(std::move(templates_path)) {
  for (std::size_t i = 0; i < template_variables.size(); ++i) {
    template_variables_value[i].resize(template_variables[i].size());
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
    if (ImGui::BeginCombo("Templates", templates_names[current_template].data())) {
      for (std::size_t i = 0; i < templates_names.size(); ++i) {
        const bool is_selected = (current_template == i);
        if (ImGui::Selectable(templates_names[i].data(), is_selected)) {
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
      for (std::size_t i = 0; i < template_variables[current_template].size(); ++i) {
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("%s", template_variables[current_template][i].data());
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
        const auto variable_value = fmt::format("##{}_value", template_variables[current_template][i].data());
        ImGui::InputText(
            variable_value.c_str(), &template_variables_value[current_template][i], ImGuiInputTextFlags_CharsNoBlank);
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
      if (std::any_of(template_variables_value[current_template].cbegin(),
                      template_variables_value[current_template].cend(),
                      [](const std::string &value) { return value.empty(); })) {
        ImGui::OpenPopup("Missing value");
        error_message = "Missing one of the template parameters";
      } else if (!std::filesystem::exists(package_path)) {
        error_message = "The path specified doesn't exists";
        ImGui::OpenPopup("Missing value");
      } else {
        inja::Environment env;
        env.add_callback("camelCaseToSnakeCase",
                         [](inja::Arguments args) { return camelCaseToSnakeCase(args[0]->get<std::string>()); });
        env.add_callback("underscoreToDash",
                         [](inja::Arguments args) { return underscoreToDash(args[0]->get<std::string>()); });
        nlohmann::json data;
        for (std::size_t i = 0; i < template_variables[current_template].size(); ++i) {
          data[template_variables[current_template][i].data()] = template_variables_value[current_template][i];
        }
        std::filesystem::create_directory(package_path + "/src");
        std::filesystem::create_directory(package_path + "/include");
        auto vcpkg_temp = env.parse_template(templates_path_ + "/imgui_templates/vcpkg.json.inja");
        env.write(vcpkg_temp, data, package_path + "/vcpkg.json");
        auto imgui_cpp_temp = env.parse_template(templates_path_ + "/imgui_templates/imgui.cpp.inja");
        env.write(imgui_cpp_temp, data, package_path + "/src/" + camelCaseToSnakeCase(data["CLASS_NAME"]) + ".cpp");
        auto imgui_hpp_temp = env.parse_template(templates_path_ + "/imgui_templates/imgui.hpp.inja");
        env.write(imgui_hpp_temp, data, package_path + "/include/" + camelCaseToSnakeCase(data["CLASS_NAME"]) + ".hpp");
        auto main_temp = env.parse_template(templates_path_ + "/imgui_templates/main.cpp.inja");
        env.write(main_temp, data, package_path + "/src/main.cpp");
        auto cmake_temp = env.parse_template(templates_path_ + "/imgui_templates/CMakeLists.txt.inja");
        env.write(cmake_temp, data, package_path + "/CMakeLists.txt");
      }
    }
    if (ImGui::BeginPopupModal("Missing value", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("%s", error_message.c_str());
      ImGui::Separator();
      if (ImGui::Button("OK", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }
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
