cmake_minimum_required(VERSION 3.17)
project(GidUralClient)

set(CMAKE_CXX_STANDARD 17)

include_directories(/usr/include/c++/Extra_Include)
include_directories(/usr/include/c++/12)

find_package(PkgConfig REQUIRED)
pkg_check_modules(rdkafka++ REQUIRED IMPORTED_TARGET rdkafka++)

add_executable(GidUralClient main.cpp
        Shared/Shared.cpp
        App/GidUralProtocol/GidUralProtocol.cpp
        App/GidUralProtocol/GidUralProtocol.h
        App/Actuator/Actuator.cpp
        App/Actuator/Actuator.h
        TcpIpClient/TcpIpClient.cpp
        TcpIpClient/TcpIpClient.h
        App/Parser/Parser.cpp
        App/Parser/Parser.h
        Settings/Settings.h
        Tests/MoqData.h
        Kafka/KafkaProducer/KafkaProducer.h
        App/NpkpValidator/NpkpValidator.h)

target_link_libraries(GidUralClient PUBLIC PkgConfig::rdkafka++)



