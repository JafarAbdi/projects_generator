FROM ubuntu:20.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update
RUN apt-get install -y curl unzip tar wget git cmake build-essential

ENV VCPKG_DIR /opt/vcpkg

RUN mkdir -p /opt
RUN git clone https://github.com/Microsoft/vcpkg.git $VCPKG_DIR
RUN cd $VCPKG_DIR && \
    ./bootstrap-vcpkg.sh -disableMetrics && \
    echo 'export PATH=$PATH:$VCPKG_DIR' >> ~/.bashrc && \
    echo 'source $VCPKG_DIR/scripts/vcpkg_completion.bash' >> ~/.bashrc

WORKDIR /root
RUN git clone git@github.com:JafarAbdi/projects_generator.git
RUN cd projects_generator && \
    mkdir -p build && \
    cd build && \
    cmake -DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake ..
