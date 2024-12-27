# Устанавливаем минимальную версию CMake
cmake_minimum_required(VERSION 3.20)

# Название проекта
project(Server)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Пути к зависимостям
include_directories(${PROJECT_SOURCE_DIR}/libpqxx/include)
include_directories(${PROJECT_SOURCE_DIR}/libs/upd_24.12.2024/tinyxml2)
include_directories(${PROJECT_SOURCE_DIR}/libs/upd_24.12.2024/json-develop/include)
include_directories("C:/Program Files/PostgreSQL/16/include")
include_directories(${PROJECT_SOURCE_DIR}/libs/upd_24.12.2024/cpp-httplib)

# Исполняемый файл
add_executable(Server 
    main.cpp
    db_helpers.cpp
    request_handler.cpp
    XML_1C_integration.h
    XML_1C_integration.cpp
)

# Подключаем libpqxx
add_subdirectory(E:/dev/VS/API/Server/libs/libpqxx-7.9.2 build-pqxx)
target_link_libraries(Server PRIVATE pqxx)

# Подключение статических библиотек
target_link_libraries(Server PRIVATE
    ws2_32
    "C:/Program Files/PostgreSQL/16/lib/libpq.lib"
)

# Подключение tinyxml2
add_library(tinyxml2 STATIC
    ${PROJECT_SOURCE_DIR}/libs/upd_24.12.2024/tinyxml2/tinyxml2.cpp
)
target_link_libraries(Server PRIVATE tinyxml2)
