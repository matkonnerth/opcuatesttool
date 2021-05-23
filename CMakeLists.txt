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
include(${CMAKE_SOURCE_DIR}/cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

add_subdirectory(testrunner)
add_subdirectory(testService)
add_subdirectory(persistence)
add_subdirectory(luaRunner)