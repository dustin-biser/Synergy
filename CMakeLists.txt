cmake_minimum_required(VERSION 2.8.4)
project(Synergy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/Linux")
#set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib/Linux")

include_directories(external/glfw)

include_directories (${OPENGL_INCLUDE_DIR})
set(LIBRARIES gmp gmpxx GL GLU glut X11 Xxf86vm Xrandr pthread Xi m Xext GLEW Xmu freetype ftgl blas)

#add_library(Synergy <all files>)
#target_link_libraries(Synergy ${GLFW_LIBRARIES})
#target_link_libraries(Synergy ${GLFW_STATIC_LIBRARIES})

#GlfwExample ===========================================
add_executable(GlfwExample examples/GlfwExample.cpp)
target_link_libraries(GlfwExample "${PROJECT_SOURCE_DIR}/lib/Linux/libglfw3.a")
target_link_libraries(GlfwExample ${LIBRARIES})
#target_link_libraries(GlfwExample LINK_PUBLIC Synergy)

