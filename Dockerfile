# 42 campus environment — Ubuntu 22.04 amd64 (matches CI + evaluation machines)
# Ubuntu 22.04 APT already ships the exact versions previously compiled from source:
#   valgrind 3.18.1, make 4.3, readline 8.1, clang-12, gcc-10
# Using APT keeps the build fast while preserving version parity with campus.
FROM ubuntu:22.04

# Prevent interactive prompts during apt installs
ENV DEBIAN_FRONTEND=noninteractive

# ── Core tools ────────────────────────────────────────────────────────────────
RUN apt-get update && apt-get install -y \
    wget curl git \
    build-essential binutils \
    gcc-10 g++-10 gcc-11 g++-11 \
    clang clang-12 clang-14 \
    lldb-12 gdb \
    cmake meson pkg-config \
    make \
    valgrind \
    libreadline-dev libreadline8 \
    python3 python3-venv python3-pip \
    zsh \
    cppcheck clangd \
    xorg libxext-dev zlib1g-dev libbsd-dev libcmocka-dev \
    && apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

# ── Compiler aliases (match campus defaults) ──────────────────────────────────
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-10 100 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 100 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-12 100 \
    && update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-12 100 \
    && update-alternatives --install /usr/bin/cc cc /usr/bin/clang-12 100 \
    && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++-12 100

# ── Norminette ────────────────────────────────────────────────────────────────
RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"
RUN pip install --upgrade pip setuptools && pip install norminette

# ── funcheck (malloc/free return-value checker) ───────────────────────────────
# "Iy" answers the two interactive prompts: choose Install, confirm Yes
RUN echo "Iy" | SHELL=/bin/bash bash -c \
    "$(curl -fsSL https://raw.githubusercontent.com/tmatis/funcheck/main/scripts/install.sh)"
ENV PATH="/root/.local/funcheck/host:$PATH"

# ── oh-my-zsh (optional, matches campus feel) ─────────────────────────────────
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" || true

WORKDIR /app
CMD ["/bin/bash"]
