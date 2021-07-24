#pragma once

#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <string>
#include <vector>

struct TemplateVariable {
  std::string name;
  std::string description;
  std::string value;
};

struct TemplateFile {
  std::string name;
  std::string output_path;  // w.r.t. output package path
};

struct Template {
  std::string name;
  std::vector<TemplateVariable> variables;
  std::vector<TemplateFile> files;
};

namespace YAML {
template <>
struct convert<TemplateVariable> {
  static bool decode(const Node& node, TemplateVariable& rhs) {
    if (!node.IsMap()) {
      return false;
    }
    rhs.name = node["name"].as<std::string>();
    rhs.value = node["default"].as<std::string>("");
    rhs.description = node["description"].as<std::string>("");
    return true;
  }
};
template <>
struct convert<TemplateFile> {
  static bool decode(const Node& node, TemplateFile& rhs) {
    if (!node.IsMap()) {
      return false;
    }
    rhs.name = node["name"].as<std::string>();
    rhs.output_path = node["output_path"].as<std::string>();
    return true;
  }
};
template <>
struct convert<Template> {
  static bool decode(const Node& node, Template& rhs) {
    if (!node.IsMap()) {
      return false;
    }
    rhs.name = node["name"].as<std::string>();
    rhs.variables = node["variables"].as<std::vector<TemplateVariable>>();
    rhs.files = node["files"].as<std::vector<TemplateFile>>();
    return true;
  }
};
}  // namespace YAML
