add_executable(testService
        TestService.cpp
        JobScheduler.cpp
        DataBase.cpp
        Config.cpp)
target_link_libraries(testService PRIVATE CONAN_PKG::cpp-httplib pthread)
target_link_libraries(testService PRIVATE CONAN_PKG::nlohmann_json)
target_link_libraries(testService PRIVATE stdc++fs)
target_link_libraries(testService PRIVATE CONAN_PKG::spdlog)
target_link_libraries(testService PRIVATE project_warnings)
target_link_libraries(testService PRIVATE project_options)

add_custom_command(
        TARGET testService POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/getScripts.sh
                ${CMAKE_BINARY_DIR}/bin)

add_custom_command(
        TARGET testService POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_CURRENT_SOURCE_DIR}/config.json
                ${CMAKE_BINARY_DIR}/bin)