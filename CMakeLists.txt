# often can not run other operator system like windows, but I give you a cmake file.
cmake_minimum_required(VERSION 3.5.0)

project(EpollFrame)

set(CMAKE_CXX_STANDARD 11)
add_executable(EpollHttp epoll_http.cpp Network.h EpollManager.cpp)
add_executable(EpollFrameTest main.cpp EpollManager.cpp Network.h)
add_executable(EpollClient Client.cpp)
