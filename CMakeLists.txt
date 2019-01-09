project(pybinpack)
cmake_minimum_required(VERSION 2.8.12)


add_subdirectory(pybind11)
pybind11_add_module(pybinpack src/main.cpp src/Rect.cpp src/MaxRectsBinPack.cpp src/BinPackManager.cpp)