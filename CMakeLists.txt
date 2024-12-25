cmake_minimum_required(VERSION 3.14)
project(Crossy_road)

set(CMAKE_CXX_STANDARD 17)

# Путь к библиотекам SFML
set(SFML_DIR "D:\\Проекты\\SFML-2.6.2-windows-gcc-13.1.0-mingw-64-bit\\SFML-2.6.2")

find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

add_executable(Crossy_road main.cpp)

# Подключение библиотек SFML
target_link_libraries(Crossy_road sfml-system sfml-window sfml-graphics)
