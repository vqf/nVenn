#!/bin/bash
c++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) nvenn2.cpp -o nvenn2$(python3 -m pybind11 --extension-suffix)
