# often can not run other operator system like windows, but I give you a cmake file.
cmake_minimum_required(VERSION 3.5.0)

project(EpollFrame)

set(CMAKE_CXX_STANDARD 11)
add_executable(EpollFrame main.cpp Server.cpp comm.cpp comm_epoll.cpp)
