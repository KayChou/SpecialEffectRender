cmake_minimum_required(VERSION 3.5)

project(main LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DCMAKE)

FIND_PACKAGE(OpenCV REQUIRED HINTS "$ENV{HOME}/lib/opencv3_4_11/share/OpenCV")

INCLUDE_DIRECTORIES("include")
INCLUDE_DIRECTORIES("include/opengl")
INCLUDE_DIRECTORIES("include/opengl/glad/include")
INCLUDE_DIRECTORIES("include/log")

set(OpenGL_GL_PREFERENCE LEGACY)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(main
  main.cpp
  render.cpp
  include/log/log.cpp
  include/log/timestamp.cpp
  include/opengl/glad/src/glad.cpp
)

target_link_libraries(main ${OpenCV_LIBS})
target_link_libraries(main glfw)
target_link_libraries(main dl)
target_link_libraries(main log4cpp)
target_link_libraries(main EGL)