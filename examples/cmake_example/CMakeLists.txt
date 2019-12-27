cmake_minimum_required(VERSION 3.13)
include(GNUInstallDirs)
project(cmake_example)
add_executable(cmake_example cmake_example.cpp)
install(TARGETS cmake_example
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
