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

if (MSVC)
  add_compile_options(/utf-8)
endif()

# executable
add_executable(reversi
  src/main.cpp
  src/Game.h src/Game.cpp
  src/GameView.h src/GameView.cpp
  src/MainWindow.h src/MainWindow.cpp src/MainWindow.ui
)
target_link_libraries(reversi Qt${QT_MAJOR_VERSION}::Widgets opengl32)
