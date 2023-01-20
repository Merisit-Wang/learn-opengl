cmake_minimum_required(VERSION 3.24)
project(learn-opengl)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGLEW_STATIC")

include_directories(
    3rd/GLFW/include
    3rd/GLEW/include
)

link_directories(
    3rd/GLFW
    3rd/GLEW
)

file(GLOB_RECURSE files src/*.cpp)
add_executable(learn-opengl ${files})
target_link_libraries(learn-opengl glfw3 Opengl32 glew32s)
