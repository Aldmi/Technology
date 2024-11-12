cmake_minimum_required(VERSION 3.16)
project(GidUralClient)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third_party/cpp-dotenv)
add_subdirectory(third_party/librdkafka)

include_directories(Extra_Include)

add_executable(GidUralClient
        src/main.cpp
        src/main.h
        src/Shared/Shared.cpp
        src/App/GidUralProtocol/GidUralProtocol.cpp
        src/App/GidUralProtocol/GidUralProtocol.h
        src/App/Actuator/Actuator.cpp
        src/App/Actuator/Actuator.h
        src/TcpIpClient/TcpIpClient.cpp
        src/TcpIpClient/TcpIpClient.h
        src/App/Parser/Parser.cpp
        src/App/Parser/Parser.h
        src/Settings/Settings.h
        src/Tests/MoqData.h
        src/Kafka/KafkaProducer/KafkaProducer.h
        src/App/NpkpValidator/NpkpValidator.h
        src/App/ActuatorVm/ActuatorVm.h
        )

target_link_libraries(GidUralClient PUBLIC rdkafka++ cpp_dotenv)



