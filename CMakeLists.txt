cmake_minimum_required(VERSION 3.22)

project(RenderHelp)

add_executable(sample_01_triangle sample_01_triangle.cpp)
add_executable(sample_02_texture sample_02_texture.cpp)
add_executable(sample_03_box sample_03_box.cpp)
add_executable(sample_04_gouraud sample_04_gouraud.cpp)
add_executable(sample_05_model sample_05_model.cpp)
add_executable(sample_06_normal sample_06_normal.cpp)
add_executable(sample_07_specular sample_07_specular.cpp)

if (WIN32)
    add_executable(sample_08_win32 WIN32 sample_08_win32.cpp)
endif ()
