cmake_minimum_required(VERSION 3.24)
project(learn-opengl)

set(CMAKE_CXX_STANDARD 17)

include_directories(3rd/GLFW/include)

link_directories(3rd/GLFW)

file(GLOB_RECURSE files src/*.cpp)
add_executable(learn-opengl ${files})
target_link_libraries(learn-opengl glfw3 Opengl32)
