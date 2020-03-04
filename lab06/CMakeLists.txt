cmake_minimum_required(VERSION 3.10)
project(sq)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} /usr/local/include)

add_executable(textures gl_arcball.cpp gl_viewer.cpp main.cpp application.cpp obj.cpp)

target_link_libraries(textures ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${GLEW_LIBRARY})
if (APPLE)
  target_link_libraries(textures /usr/local/Cellar/glew/2.1.0_1/lib/libGLEW.a 
                        /usr/local/Cellar/freeglut/3.0.0/lib/libglut.a)
endif()