cmake_minimum_required(VERSION 3.9.2)

project(interactive-secondary-motion)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -w -Wextra") # -Wall

set(INCLUDE_DIR interactive-secondary-motion/include)
set(SOURCE_DIR interactive-secondary-motion/src)

install(DIRECTORY interactive-secondary-motion/resource/
        DESTINATION ${CMAKE_BINARY_DIR}/resource
        FILES_MATCHING PATTERN "*.dae"  PATTERN "*.frag" PATTERN "*.vert")

include_directories(${INCLUDE_DIR})

file(GLOB_RECURSE SOURCES
    ${INCLUDE_DIR}/*.h
    ${SOURCE_DIR}/*.cpp
)

# message(${SOURCES})

# file(GLOB_RECURSE ZOURCES
#   Camera.cpp
#   CoordGrid.cpp
#   Mesh.cpp
#   Model.cpp
#   ModelViewProjectionShader.cpp
#   RigidSphere.cpp
#   Shader.cpp
# )

# message(${ZOURCES})

add_executable(interactive-secondary-motion ${SOURCES})

add_subdirectory(vendor)

include_directories(
  ${GLEW_INCLUDE}
  ${GLFW_INCLUDE}
  ${GLM_INCLUDE}
  ${ASSIMP_INCLUDE}
  ${ANTTWEAKBAR_INCLUDE})

target_link_libraries(interactive-secondary-motion
  ${GLEW_LIB}
  ${GLFW_LIB}
  ${ASSIMP_LIB}
  ${ANTTWEAKBAR_LIB})

if(APPLE)

target_link_libraries(interactive-secondary-motion
  "-framework OpenGL"
  "-framework Cocoa"
  "-framework IOKit"
  "-framework CoreFoundation"
  "-framework CoreVideo")

endif(APPLE)

# add_executable(
#     ${INCLUDE_DIR}/BendingConstraint.h
#     ${INCLUDE_DIR}/Camera.h
#     ${INCLUDE_DIR}/CollisionConstraint.h
#     ${INCLUDE_DIR}/CoordGrid.h
#     ${INCLUDE_DIR}/DistanceConstraint.h
#     ${INCLUDE_DIR}/FFMPEGRecorder.h
#     ${INCLUDE_DIR}/Mesh.h
#     ${INCLUDE_DIR}/Model.h
#     ${INCLUDE_DIR}/ModelViewProjectionShader.h
#     ${INCLUDE_DIR}/RigidSphere.h
#     ${INCLUDE_DIR}/Shader.h
#     ${INCLUDE_DIR}/TwVars.h

#     ${SOURCE_DIR}/Camera.cpp
#     ${SOURCE_DIR}/CoordGrid.cpp
#     ${SOURCE_DIR}/Mesh.cpp
#     ${SOURCE_DIR}/Model.cpp
#     ${SOURCE_DIR}/ModelViewProjectionShader.cpp
#     ${SOURCE_DIR}/RigidSphere.cpp
#     ${SOURCE_DIR}/Shader.cpp
# )
