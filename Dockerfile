FROM tesnix-base:latest

WORKDIR /tesnix

COPY . /tesnix/

RUN make

ENTRYPOINT [ "make", "run" ]
