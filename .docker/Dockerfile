FROM ubuntu:20.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
    apt-get install -y curl unzip tar wget git cmake build-essential rsync clang gdb ssh && \
    # Install OpenGL
    apt-get install -y xorg-dev libgl1-mesa-dev libglu1-mesa-dev && \
    apt-get clean

ENV VCPKG_DIR /opt/vcpkg

RUN mkdir -p /opt
RUN git clone https://github.com/JafarAbdi/vcpkg.git $VCPKG_DIR
RUN cd $VCPKG_DIR && \
    # Use the same version as in build_cmake.yml
    git checkout 660b6ebf0158b456046c8b4ebc0334db34a1c9f6 && \
    ./bootstrap-vcpkg.sh -disableMetrics && \
    echo 'export PATH=$PATH:$VCPKG_DIR' >> ~/.bashrc && \
    echo 'source $VCPKG_DIR/scripts/vcpkg_completion.bash' >> ~/.bashrc

WORKDIR /root
RUN git clone https://github.com/JafarAbdi/projects_generator.git
RUN cd projects_generator && \
    mkdir -p build && \
    cd build && \
    cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake .. && \
    make -j$(nproc)
