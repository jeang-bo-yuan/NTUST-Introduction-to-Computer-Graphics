cmake_minimum_required(VERSION 3.5)
project(reversi LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QT
set(QT_MAJOR_VERSION 5)
find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# executable
add_executable(reversi
  src/main.cpp
  src/Game.h
  src/Game.cpp
)
target_link_libraries(reversi Qt${QT_MAJOR_VERSION}::Widgets)
