cmake_minimum_required(VERSION 3.0)
project(opcuaPerfTool)

include(/opt/conan-cmake/conan.cmake)

conan_cmake_run(
	CONANFILE conanfile.txt
	UPDATE
	BASIC_SETUP
	BUILD missing
	ARCH ${CMAKE_SYSTEM_PROCESSOR}
)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
#set_project_warnings(project_warnings)

add_subdirectory(open62541)

add_executable(opcuaPerfTool
    driver/TestRunner.cpp
    comm/Comm.cpp
    comm/Client.cpp
	job/Job.cpp
	job/Task.cpp)
target_include_directories(opcuaPerfTool PRIVATE comm)
target_include_directories(opcuaPerfTool PRIVATE job)
target_link_libraries(opcuaPerfTool PRIVATE project_warnings)
target_link_libraries(opcuaPerfTool PRIVATE open62541)


add_executable(HttpDriver driver/httpDriver.cpp)
target_link_libraries(HttpDriver PRIVATE ${CONAN_LIBS})