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

add_subdirectory(open62541)

add_executable(testRunner
    driver/TestRunner.cpp
    comm/Comm.cpp
    comm/Client.cpp
	job/Job.cpp
    job/Task.cpp
    job/JobFactory.cpp)
target_include_directories(testRunner PRIVATE comm)
target_include_directories(testRunner PRIVATE job)
target_link_libraries(testRunner PRIVATE project_warnings)
target_link_libraries(testRunner PRIVATE open62541 CONAN_PKG::jsonformoderncpp)

add_executable(TestService 
    TestService/HttpServer.cpp
    TestService/JobScheduler.cpp)
target_link_libraries(TestService PRIVATE CONAN_PKG::pistache pthread)