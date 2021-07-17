#include <gflags/gflags.h>

#include "projects_generator.hpp"

DEFINE_string(templates_path, ".", "The path for the templates directory -- default current directory");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  ProjectsGenerator projects_generator(fLS::FLAGS_templates_path);
  projects_generator.run();
  return 0;
}
