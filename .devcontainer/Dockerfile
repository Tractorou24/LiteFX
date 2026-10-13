
FROM ubuntu:26.04

# Global variables
ENV DEBIAN_FRONTEND=noninteractive
ENV DEBCONF_NONINTERACTIVE_SEEN=true
ENV VULKAN_SDK=/usr

# Install basic utilities
RUN echo 'tzdata tzdata/Areas select Etc' | debconf-set-selections;     \
    echo 'tzdata tzdata/Zones/Etc select UTC' | debconf-set-selections; \
    apt-get update &&                                                   \
    apt-get install -y --no-install-recommends                          \
    # Basic utilities
    ca-certificates                                                     \
    curl                                                                \
    wget                                                                \
    gnupg                                                               \
    git                                                                 \
    zip                                                                 \
    unzip                                                               \
    xz-utils                                                            \
    python3                                                             \
    python3-pip                                                         \
    python3-venv                                                        \
    && rm -rf /var/lib/apt/lists/*

# Setup additional repositories
RUN mkdir -p /etc/apt/keyrings &&                                                                           \
    curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor -o /etc/apt/keyrings/llvm.gpg &&  \
    echo "deb [signed-by=/etc/apt/keyrings/llvm.gpg] https://apt.llvm.org/resolute/ llvm-toolchain-resolute-23 main" > /etc/apt/sources.list.d/llvm23.list

# Install other dependencies
RUN apt-get update &&                                                   \
    apt-get install -y --no-install-recommends                          \
    # Compilers and build tools
    build-essential                                                     \
    gcc-16                                                              \
    g++-16                                                              \
    cmake                                                               \
    ninja-build                                                         \
    pkg-config                                                          \
    autoconf                                                            \
    libtool                                                             \
    clang-23                                                            \
    clangd-23                                                           \
    lld-23                                                              \
    lldb-23                                                             \
    libc++-23-dev                                                       \
    libc++abi-23-dev                                                    \
    libclang-rt-23-dev                                                  \
    # Development libraries for graphics and windowing
    xorg-dev                                                            \
    freeglut3-dev                                                       \
    libx11-xcb-dev                                                      \
    libxcb-dri3-dev                                                     \
    libxcb-randr0-dev                                                   \
    libxcb-xinerama0-dev                                                \
    libxcb-xkb-dev                                                      \
    libxkbcommon-dev                                                    \
    libxkbcommon-x11-dev                                                \
    libudev-dev                                                         \
    libdbus-1-dev                                                       \
    libgl-dev                                                           \
    libegl-dev                                                          \
    libwayland-dev                                                      \
    xvfb                                                                \
    # Vulkan SDK and related tools
    libvulkan-dev                                                       \
    mesa-vulkan-drivers                                                 \
    vulkan-tools                                                        \
    vulkan-validationlayers                                             \
    glslang-tools                                                       \
    && rm -rf /var/lib/apt/lists/*

# Install directx shader compiler (dxc)
ENV DXC_VERSION=1.9.2609
ENV DXC_RELEASE_DATE=2026_09_28
RUN curl -fsSL "https://github.com/microsoft/DirectXShaderCompiler/releases/download/v${DXC_VERSION}/linux_dxc_${DXC_RELEASE_DATE}.x86_x64.tar.gz" | tar -xz -C /usr/local

# Make the desired tools the default versions
RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-23 100 &&         \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-23 100 &&   \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-16 100 &&               \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-16 100 &&               \
    update-alternatives --install /usr/bin/lld lld /usr/bin/lld-23 100 &&               \
    update-alternatives --install /usr/bin/lldb lldb /usr/bin/lldb-23 100 &&            \
    update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-23 100 

# Install VCPKG
ENV VCPKG_ROOT=/opt/vcpkg
ENV VCPKG_DEFAULT_BINARY_CACHE=/opt/vcpkg_cache
RUN git clone https://github.com/microsoft/vcpkg.git ${VCPKG_ROOT} &&   \
    ${VCPKG_ROOT}/bootstrap-vcpkg.sh -disableMetrics &&                 \
    mkdir -p ${VCPKG_DEFAULT_BINARY_CACHE}

ENV PATH="${VCPKG_ROOT}:${PATH}"

WORKDIR /workspace
CMD ["/bin/bash"]
