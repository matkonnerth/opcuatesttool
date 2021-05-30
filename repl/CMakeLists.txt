
find_package(Lua REQUIRED)
add_executable(repl console.cpp)
#find_package(modernOpc REQUIRED)
#find_package(open62541 REQUIRED)
target_link_libraries(repl PRIVATE project_warnings)
target_link_libraries(repl PRIVATE project_options)
target_link_libraries(repl PRIVATE CONAN_PKG::sol2)
#target_link_libraries(luaRunner PRIVATE modernOpc open62541::open62541)
target_link_libraries(repl PRIVATE CONAN_PKG::spdlog)
target_link_libraries(repl PRIVATE CONAN_PKG::lua)


add_executable(replService server.cpp)
target_link_libraries(replService PRIVATE project_warnings)
target_link_libraries(replService PRIVATE project_options)
target_link_libraries(replService PRIVATE CONAN_PKG::spdlog)
target_link_libraries(replService PRIVATE CONAN_PKG::cpp-httplib pthread)
target_link_libraries(replService PRIVATE CONAN_PKG::nlohmann_json)