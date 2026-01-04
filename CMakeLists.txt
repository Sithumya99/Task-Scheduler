cmake_minimum_required(VERSION 3.20)

project(task-scheduler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task-scheduler
    task-scheduler-service/main.cpp
    task-scheduler-service/http_tcpServer.cpp
)