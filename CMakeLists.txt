cmake_minimum_required(VERSION 3.20)
project(INF2223 Proyecto EDD C)

# Usar C90 (ANSI C)
set(CMAKE_C_STANDARD 90)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Indica qué archivo fuente compilar
add_executable(INF2223 Proyecto EDD main.c)
