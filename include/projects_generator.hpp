#pragma once

// clang-format off
#include "imgui.h"
#include <imfilebrowser.h>
// clang-format on

#include <array>
#include <string_view>
#include <templates.hpp>
#include <unordered_map>
#include <vector>

struct GLFWwindow;

class ProjectsGenerator {
 public:
  explicit ProjectsGenerator(std::string templates_path);

  ~ProjectsGenerator();

  void run();

 private:
  GLFWwindow *window_;
  std::vector<Template> templates;
  std::size_t current_template = 0;
  ImGui::FileBrowser file_browser;
  std::string package_path;
  std::string error_message;
  const std::filesystem::path templates_path_;
};
