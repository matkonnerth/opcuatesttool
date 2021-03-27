add_executable(chaiRunner 
    main.cpp
    ../comm/Comm.cpp
    ../comm/Client.cpp
)

target_link_libraries(chaiRunner 
    PRIVATE 
        CONAN_PKG::chaiscript
        CONAN_PKG::spdlog
        CONAN_PKG::open62541
)

target_link_libraries(chaiRunner 
    PRIVATE 
        project_options
        project_warnings
)