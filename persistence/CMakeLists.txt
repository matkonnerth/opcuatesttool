add_executable(influxDBImporter
    InfluxDBImporter.cpp
)
target_link_libraries(influxDBImporter PRIVATE CONAN_PKG::libcurl pthread)
target_link_libraries(influxDBImporter PRIVATE CONAN_PKG::nlohmann_json)
target_link_libraries(influxDBImporter PRIVATE project_options)
target_link_libraries(influxDBImporter PRIVATE CONAN_PKG::influxdb-cpp)
target_link_libraries(influxDBImporter PRIVATE project_warnings)
target_link_libraries(influxDBImporter PRIVATE CONAN_PKG::spdlog)