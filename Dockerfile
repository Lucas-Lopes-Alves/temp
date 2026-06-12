FROM ubuntu:noble

WORKDIR /tesnix

COPY . .

ENV PATH=/tesnix/cross/bin/:${PATH} DISPLAY=:0

RUN apt-get update && apt-get install \ 
 qemu-system-x86 \
 grub-pc-bin \ 
 make \
 xorriso \
 libisl23 \
 libmpc3 \
 libsdl2-2.0-0 \
 libgtk-3-0 -y \
 && rm -rf /var/lib/apt/lists/*

RUN make

CMD [ "bash" ]
