cmake_minimum_required (VERSION 2.8.11)
project (osort)
add_executable (osort main.cpp)
set_property(TARGET osort PROPERTY
  MSVC_RUNTIME_LIBRARY "MultiThreaded")
