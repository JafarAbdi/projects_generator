#pragma once
#include <array>
#include <string_view>
#include <unordered_map>
#include <vector>

struct GLFWwindow;

class ProjectsGenerator {
 public:
  ProjectsGenerator();

  ~ProjectsGenerator();

  void run();

 private:
  GLFWwindow *window_;
  std::vector<std::string_view> templates_names = {"imgui"};
  std::vector<std::vector<std::string_view>> template_variables = {{"PROJECT_NAME", "CLASS_NAME"}};
  std::vector<std::vector<std::string>> template_variables_value;
  std::size_t current_template = 0;
};
