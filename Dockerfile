# Builds CDK21 and RTS7, then the At compiler; runs the test suite and
# builds the examples:
#
#   docker build -t at .                     # fails if any test fails
#   docker run --rm -v "$PWD":/w -w /w at at prog.at   # use the compiler
#
# The toolchain is 32-bit x86 (RTS7 is i386 assembly), hence linux/amd64.
FROM --platform=linux/amd64 debian:trixie AS toolchain
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential g++-multilib flex libfl-dev bison yasm perl python3 \
      bzip2 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /opt
# CDK21 and RTS7 are vendored (see vendor/README.md)
COPY vendor/Libcdk21-202605161833.tar.bz2 cdk.tar.bz2
COPY vendor/Librts7-202605092235.tar.bz2 rts.tar.bz2
RUN echo "8cdc0e726eea9b89246f94036cf8c2dc4025a331c7170028c9f111633bfd878d  cdk.tar.bz2" | sha256sum -c \
 && echo "e89a80ad31630396f14ddf1dccfbff8efd9002985b63cb3692759aaabc670bc0  rts.tar.bz2" | sha256sum -c \
 && tar xjf cdk.tar.bz2 && tar xjf rts.tar.bz2
# skip the documentation (doxygen) step
RUN cd libcdk21-* && touch doc/.docsdone && make -j"$(nproc)" install ROOT=
RUN cd librts7-* && make -j"$(nproc)" install PREFIX=

# the compiler and the examples ("docker build --target build" stops here)
FROM toolchain AS build
WORKDIR /src
COPY . .
RUN make clean && make -j"$(nproc)" && install -m 755 build/at /usr/local/bin/at
RUN make examples

# the default target also runs the tests, and fails if one fails
FROM build AS test
RUN tests/run.sh
