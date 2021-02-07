cmake_minimum_required (VERSION 3.15)
project (osort)
add_executable (osort main.cpp)
set_property(TARGET osort PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
