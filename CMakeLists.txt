cmake_minimum_required(VERSION 3.16)

set(CMAKE_C_COMPILER "C:/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe")

project(YaltaChess)
set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/SFML/lib/cmake/SFML")

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(YaltaChess src/main.cpp)
target_include_directories(YaltaChess PRIVATE include)
target_link_libraries(YaltaChess SFML::Graphics SFML::Window SFML::System)