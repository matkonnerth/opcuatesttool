# build with
# sudo docker build -t matkonnerth/opcuatesttool .
# run with
# sudo docker run --network host --mount source=testTool-vol,destination=/opt/testService/bin/jobs matkonnerth/opcuatesttool
# path to git repo
# sudo docker run --network host --mount source=testTool-vol,destination=/opt/testService/bin/jobs matkonnerth/opcuatesttool https://github.com/matkonnerth/opcuaTestToolScripts.git
# uses host network and a volume for the jobs
#
# upload
#  docker push ghcr.io/matkonnerth/opcuatesttool:latest

FROM debian:bullseye AS cpp-build-base
RUN apt-get update \
    && apt-get -y install git \
    && apt-get -y install libxml2-dev \
    && apt-get -y install build-essential \
    && apt-get -y install cmake \
    && apt-get -y install python3 \
    && apt-get -y install python3-pip \
    && apt-get -y install clang \
    && pip install conan;

FROM cpp-build-base AS build

#open62541
RUN git clone https://github.com/open62541/open62541.git open62541 \
    && cd open62541 \
    #&& git checkout tags/v1.2.2 \
    && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=ON -DUA_ENABLE_SUBSCRIPTIONS_EVENTS=ON .. \
    && make -j \
    && make install;

#nodesetLoader
RUN git clone https://github.com/matkonnerth/nodesetLoader.git nodesetLoader \
    && cd nodesetLoader && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_CONAN=OFF -DBUILD_SHARED_LIBS=ON -DENABLE_BACKEND_OPEN62541=ON .. \
    && make -j \
    && make install;

#modernOpc
RUN git clone https://github.com/matkonnerth/ModernOPC.git modernOpc \
    && cd modernOpc && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_CONAN=ON -DBUILD_SHARED_LIBS=ON .. \
    && make -j \
    && make install;

#use clang
RUN update-alternatives --install /usr/bin/cc cc /usr/bin/clang 100 \
    && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100;

WORKDIR /src

COPY CMakeLists.txt conanfile.txt ./
COPY cmake ./cmake
COPY testService ./testService
COPY testrunner ./testrunner
COPY api ./api
COPY persistence ./persistence
COPY repl ./repl

RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. \
    && make testService -j \
    && make testRunner -j \
    && make repl -j;

#build webapp
FROM node:12-alpine as build-webapp
RUN mkdir -p /app

WORKDIR /app

COPY hmi/testToolApp/package.json /app

RUN npm install
COPY hmi/testToolApp/. /app
RUN npm run build

FROM debian:bullseye

WORKDIR /opt/testService

COPY --from=build /src/build/bin ./bin
#get shared libs
COPY --from=build /usr/local/lib/libopen62541.* /usr/local/lib/
COPY --from=build /usr/local/lib/libmodernOpc.so /usr/local/lib
COPY --from=build /usr/local/lib/libNodesetLoader.so /usr/local/lib
COPY --from=build-webapp /app/dist ./bin/dist

RUN apt-get update \
    && apt-get -y install git \
    && apt-get -y install libxml2;

RUN ldconfig;

CMD ["/opt/testService/bin/testService"]

