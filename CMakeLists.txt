cmake_minimum_required(VERSION 3.16)
project(KCM_rebuild)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")
add_executable(KCM_rebuild main.cpp alg/alg.cpp alg/multi_Thread.cpp structure/Graph.cpp structure/type.h)
