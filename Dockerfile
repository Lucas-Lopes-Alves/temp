FROM tesnix-base:latest

WORKDIR /tesnix

ENV PATH=/cross/bin/:${PATH}

COPY . /tesnix/

RUN make

CMD [ "bash" ]
