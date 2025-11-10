FROM fedora:40

RUN dnf install -y \
    mingw64-gcc mingw64-gcc-c++ mingw64-winpthreads-static \
    mingw64-headers mingw64-crt mingw64-binutils \
    cmake ninja-build git && \
    dnf clean all

WORKDIR /work
