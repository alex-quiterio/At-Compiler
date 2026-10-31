# Builds CDK21 and RTS7, then the At compiler, and runs the test suite:
#
#   docker build -t at .                     # fails if any test fails
#   docker run --rm -v "$PWD":/w -w /w at at prog.at   # use the compiler
#
# The toolchain is 32-bit x86 (RTS7 is i386 assembly), hence linux/amd64.
FROM --platform=linux/amd64 debian:trixie AS toolchain
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential g++-multilib flex libfl-dev bison yasm perl python3 \
      curl ca-certificates bzip2 \
    && rm -rf /var/lib/apt/lists/*
ARG WIKI=https://robots.hlt.inesc-id.pt/w/pt/images
WORKDIR /opt
RUN curl -fsSL -o cdk.tar.bz2 $WIKI/5/5f/Libcdk21-202605161833.tar.bz2 \
 && curl -fsSL -o rts.tar.bz2 $WIKI/3/39/Librts7-202605092235.tar.bz2 \
 && echo "8cdc0e726eea9b89246f94036cf8c2dc4025a331c7170028c9f111633bfd878d  cdk.tar.bz2" | sha256sum -c \
 && echo "e89a80ad31630396f14ddf1dccfbff8efd9002985b63cb3692759aaabc670bc0  rts.tar.bz2" | sha256sum -c \
 && tar xjf cdk.tar.bz2 && tar xjf rts.tar.bz2
# skip the documentation (doxygen) step
RUN cd libcdk21-* && touch doc/.docsdone && make -j"$(nproc)" install ROOT=
RUN cd librts7-* && make -j"$(nproc)" install PREFIX=

FROM toolchain
WORKDIR /src
COPY . .
RUN make clean && make -j"$(nproc)" && install -m 755 at /usr/local/bin/at
RUN tests/run.sh
