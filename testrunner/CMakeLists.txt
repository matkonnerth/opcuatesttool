find_package(open62541 REQUIRED)
find_package(modernopc REQUIRED)

add_executable(testRunner
    TestRunner.cpp
	Job.cpp
    JobFactory.cpp
    Runtime.cpp)

target_link_libraries(testRunner PRIVATE modernopc)
target_link_libraries(testRunner PRIVATE CONAN_PKG::nlohmann_json)
target_link_libraries(testRunner PRIVATE CONAN_PKG::spdlog)
target_link_libraries(testRunner PRIVATE CONAN_PKG::sol2)
target_link_libraries(testRunner PRIVATE project_warnings)
target_link_libraries(testRunner PRIVATE project_options)