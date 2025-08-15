cmake_minimum_required(VERSION 3.15...4.0)
project(nvenn2 LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(nvenn2 src/nvenn2.cpp)
install(TARGETS nvenn2 DESTINATION .)
