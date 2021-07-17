cmake_minimum_required(VERSION 3.16.3)
project(projects_generator)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(
  -Wall
  -Wextra
  -Wwrite-strings
  -Wunreachable-code
  -Wpointer-arith
  -Wredundant-decls
  -Wcast-qual)

find_package(imgui REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(spdlog REQUIRED)
find_package(inja REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(gflags REQUIRED)

include(FetchContent)
FetchContent_Declare(
  imgui-filebrowser
  GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser.git
  GIT_TAG master)
FetchContent_MakeAvailable(imgui-filebrowser)
add_library(imgui-filebrowser INTERFACE)
target_include_directories(imgui-filebrowser INTERFACE ${imgui-filebrowser_SOURCE_DIR})

add_library(projects_generator_lib SHARED src/projects_generator.cpp src/utils.cpp include/utils.hpp)
target_include_directories(projects_generator_lib PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                                         $<INSTALL_INTERFACE:include>)
target_link_libraries(
  projects_generator_lib
  PUBLIC imgui::imgui
         glfw
         GLEW::GLEW
         spdlog::spdlog
         spdlog::spdlog_header_only
         pantor::inja
         nlohmann_json
         nlohmann_json::nlohmann_json
         stdc++fs
         fmt::fmt
         imgui-filebrowser)
set_target_properties(projects_generator_lib PROPERTIES SOVERSION 0 VERSION 0.0.1)

add_executable(projects_generator src/main.cpp)
target_link_libraries(projects_generator PRIVATE projects_generator_lib gflags)
target_include_directories(projects_generator PRIVATE include)

include(GNUInstallDirs)
install(
  TARGETS projects_generator projects_generator_lib
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES resources/projects_generator.desktop DESTINATION share/applications/)
install(FILES resources/projects_generator.png DESTINATION share/icons/hicolor/128x128/apps)
install(DIRECTORY imgui_templates DESTINATION share)
