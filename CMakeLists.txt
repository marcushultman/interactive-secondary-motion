cmake_minimum_required(VERSION 3.9.2)

project(interactive-secondary-motion)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -w -Wextra") # -Wall
add_compile_options("-stdlib=libc++")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

add_subdirectory(vendor)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/interactive-secondary-motion/include/config.h.in
  ${CMAKE_BINARY_DIR}/include/config.h)

set(FILES
  Camera.cpp
  CoordGrid.cpp
  main.cpp
  Mesh.cpp
  Model.cpp
  ModelViewProjectionShader.cpp
  RigidSphere.cpp
  Shader.cpp
)

foreach(f ${FILES})
  list(APPEND SOURCES "interactive-secondary-motion/src/${f}")
endforeach()

add_executable(interactive-secondary-motion ${SOURCES})

target_include_directories(
  interactive-secondary-motion
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/interactive-secondary-motion/include
  ${CMAKE_BINARY_DIR}/include
  ${OPENGL_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${GLFW_INCLUDE}
  ${GLM_INCLUDE}
  ${ASSIMP_INCLUDE}
  ${ANTTWEAKBAR_INCLUDE}
  ${CMAKE_BINARY_DIR}/include
)

target_link_libraries(
  interactive-secondary-motion
  ${OPENGL_LIBRARIES}
  /usr/local/lib/libGLEW.a
  glfw
  /usr/local/lib/libassimp.dylib
  ${ANTTWEAKBAR_LIB}
)

if(APPLE)

target_link_libraries(interactive-secondary-motion
  "-framework Cocoa"
)

endif(APPLE)
