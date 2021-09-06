# Projects generator
Generate packages/projects from templates

# CI
[![Build](https://github.com/JafarAbdi/projects_generator/actions/workflows/build_cmake.yml/badge.svg)](https://github.com/JafarAbdi/projects_generator/actions/workflows/build_cmake.yml)
[![Format](https://github.com/JafarAbdi/projects_generator/actions/workflows/format.yml/badge.svg)](https://github.com/JafarAbdi/projects_generator/actions/workflows/format.yml)

# Build AppImage locally

```
./scripts/build_appimage.sh
```

### Run the program
```
./projects_generator-x86_64.AppImage
```

NOTE: If you got an error about `libGL.so.1` not found, you need to install `apt install libglu1-mesa`

### Run the program with different templates
```
./projects_generator-x86_64.AppImage --templates_path=/path_to_templates_dir
./projects_generator-x86_64.AppImage --git_url=git@github.com:USER/REPO.git
./projects_generator-x86_64.AppImage --git_url=https://github.com/USER/REPO.git
```

### Debug the appimage program
```
./projects_generator-x86_64.AppImage --appimage-mount # cd /tmp/.mount_######
```

# Develope inside docker image
```
docker pull ghcr.io/jafarabdi/projects_generator:main
docker run -it ghcr.io/jafarabdi/projects_generator:main
./projects_generator/build/projects_generator --templates_path=./projects_generator/templates/
```
