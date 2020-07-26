cmake_minimum_required(VERSION 3.0)
project(opcuaPerfTool)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD missing)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_subdirectory(open)

add_executable(testRunner
    testRunner/TestRunner.cpp
    comm/Comm.cpp
    comm/Client.cpp
	job/Job.cpp
    job/Task.cpp
    job/JobFactory.cpp
    job/VariantWrapper.cpp
    job/Expectation.cpp
    job/Result.cpp)
target_include_directories(testRunner PRIVATE comm)
target_include_directories(testRunner PRIVATE job)
target_link_libraries(testRunner PRIVATE project_warnings)
target_link_libraries(testRunner PRIVATE open62541 CONAN_PKG::jsonformoderncpp)
target_link_libraries(testRunner PRIVATE project_options)
target_link_libraries(testRunner PRIVATE CONAN_PKG::spdlog)

add_executable(testService 
    testService/TestService.cpp
    testService/JobScheduler.cpp
    testService/DataBase.cpp)
target_link_libraries(testService PRIVATE CONAN_PKG::pistache pthread)
target_link_libraries(testService PRIVATE project_options)
target_link_libraries(testService PRIVATE stdc++fs)
target_link_libraries(testService PRIVATE CONAN_PKG::spdlog)

add_executable(influxDBImporter
    persistence/InfluxDBImporter.cpp
)
target_link_libraries(influxDBImporter PRIVATE CONAN_PKG::libcurl pthread)
target_link_libraries(influxDBImporter PRIVATE open62541 CONAN_PKG::jsonformoderncpp)
target_link_libraries(influxDBImporter PRIVATE project_options)
target_link_libraries(influxDBImporter PRIVATE CONAN_PKG::InfluxDB)
target_link_libraries(influxDBImporter PRIVATE CONAN_PKG::spdlog)

add_executable(testInfluxDb testInflux.cpp)
target_link_libraries(testInfluxDb PRIVATE CONAN_PKG::InfluxDB)
target_link_libraries(testInfluxDb PRIVATE project_options)


add_executable(testVariant testVariant.cpp)
target_link_libraries(testVariant PRIVATE open62541)
target_link_libraries(testVariant PRIVATE project_options)