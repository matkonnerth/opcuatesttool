# build with
# sudo docker build -t matkonnerth/opcuatesttool .
# run with
# sudo docker run --network host --mount source=testTool-vol,destination=/usr/local/bin/jobs matkonnerth/opcuatesttool
# uses host network and a volume for the jobs

FROM debian:bullseye AS runtime
RUN set -ex; \
    apt-get update; \
    apt-get -y install git; \
    apt-get -y install libxml2;

COPY ./build/bin /usr/local/bin

#okay, this is ugly
COPY ./libs/libmodernOpc.so /usr/local/lib
COPY ./libs/libopen62541.so.1 /usr/local/lib
COPY ./libs/libNodesetLoader.so /usr/local/lib

RUN ldconfig

CMD ["/usr/local/bin/testService"]

