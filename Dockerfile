# build with
# sudo docker build -t matkonnerth/opcuatesttool .
# run with
# sudo docker run --network host --mount source=testTool-vol,destination=/opt/testService/bin/jobs matkonnerth/opcuatesttool
# uses host network and a volume for the jobs
# uses the image specified in DockerfileBuild

FROM cpp-build-base:0.1.0 AS build

#open62541
RUN git clone https://github.com/open62541/open62541.git open62541 \
    && cd open62541 && mkdir build && cd build \
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

RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. \
    && make testService -j \
    && make testRunner -j;

FROM debian:bullseye

RUN set -ex; \
    apt-get update; \
    apt-get -y install libxml2;

WORKDIR /opt/testService

COPY --from=build /src/build/bin ./bin
#get shared libs
COPY --from=build /usr/local/lib/libopen62541.so.1 /usr/local/lib
COPY --from=build /usr/local/lib/libmodernOpc.so /usr/local/lib
COPY --from=build /usr/local/lib/libNodesetLoader.so /usr/local/lib
COPY hmi/testToolApp/dist ./bin/dist

RUN ldconfig

RUN set -ex; \
    apt-get update; \
    apt-get -y install git;

CMD ["/opt/testService/bin/testService"]

