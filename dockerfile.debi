FROM debian:trixie-slim AS build

RUN apt-get update && apt-get install -y \
build-essential \
wget \
bison \
flex \
libgmp3-dev \
libmpfr-dev \
libmpc-dev \
libncurses-dev \
libexpat1-dev \
zlib1g-dev \
&& rm -rf /var/lib/apt/lists/* \
&& mkdir -p /download /cross

# Binutils build, start
WORKDIR /src

ENV PATH=/cross/bin:${PATH}

RUN wget -nv https://ftp.gnu.org/gnu/binutils/binutils-2.46.0.tar.xz -P /download/ \
&& tar -xf /download/binutils-2.46.0.tar.xz

WORKDIR /build/binutils

RUN /src/binutils-2.46.0/configure \
--target=i686-elf \
--prefix=/cross \
--with-sysroot \
--disable-nls \
--disable-werror \
--disable-multilib

RUN make -j$(nproc)
RUN make install
#Binutils build, end

#GCC build, start
WORKDIR /src

RUN wget -nv https://mirror.team-cymru.com/gnu/gcc/gcc-16.1.0/gcc-16.1.0.tar.xz -P /download \
&& tar -xf /download/gcc-16.1.0.tar.xz

WORKDIR /build/gcc

RUN /src/gcc-16.1.0/configure \
--target=i686-elf \
--prefix=/cross \
--disable-nls \
--enable-languages=c \
--without-headers \
--disable-multilib
RUN make -j$(nproc) all-gcc
RUN make -j$(nproc) all-target-libgcc
RUN make install-gcc
RUN make install-target-libgcc
#GCC build, end

#GDB build, start
WORKDIR /src

RUN wget -nv https://mirror.team-cymru.com/gnu/gdb/gdb-17.2.tar.xz -P /download \
&& tar -xf /download/gdb-17.2.tar.xz

WORKDIR /build/gdb

RUN /src/gdb-17.2/configure \
--target=i686-elf \
--prefix=/cross \
--with-sysroot \
--disable-nls \
--disable-werror \
--without-python
RUN make -j$(nproc) all-gdb
RUN make install-gdb

#GDB build, end

#---------------------------------------------------------------------------#

FROM debian:trixie-slim

RUN echo "PS1='\[\e[0;32m\]\u@\h:\w\$\[\e[0m\] '" > /root/.bashrc \
&& apt-get update && apt-get install -y \
qemu-system-x86 \
grub-pc-bin \
make \
xorriso \
libmpc3 \
libsdl2-2.0-0 \
libgtk-3-0 \
git \
&& rm -rf /var/lib/apt/lists/*

COPY --from=build /cross /cross

ENV PATH=/cross/bin:${PATH}

WORKDIR /tesnix

COPY . .

ENTRYPOINT [ "bash" ]
