include(ExternalProject)

set_property(DIRECTORY PROPERTY EP_PREFIX ${CMAKE_BINARY_DIR}/vendor)

set(VENDOR ${CMAKE_CURRENT_SOURCE_DIR})

ExternalProject_Add(glew
  GIT_REPOSITORY https://github.com/nigels-com/glew.git
  GIT_TAG glew-2.1.0
  SOURCE_DIR ${VENDOR}/glew
  PATCH_COMMAND make -C auto
  SOURCE_SUBDIR build/cmake
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS <BINARY_DIR>/lib/libGLEW.a)
ExternalProject_Get_Property(glew SOURCE_DIR BINARY_DIR)
set(GLEW_INCLUDE ${SOURCE_DIR}/include PARENT_SCOPE)
set(GLEW_LIB ${BINARY_DIR}/lib/libGLEW.a PARENT_SCOPE)

ExternalProject_Add(glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 3.2.1
  SOURCE_DIR ${VENDOR}/glfw
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS <BINARY_DIR>/src/libglfw3.a)
ExternalProject_Get_Property(glfw SOURCE_DIR BINARY_DIR)
set(GLFW_INCLUDE ${SOURCE_DIR}/include PARENT_SCOPE)
set(GLFW_LIB ${BINARY_DIR}/src/libglfw3.a PARENT_SCOPE)

ExternalProject_Add(glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 0.9.8.5
  SOURCE_DIR ${VENDOR}/glm
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")
ExternalProject_Get_Property(glm SOURCE_DIR)
set(GLM_INCLUDE ${SOURCE_DIR} PARENT_SCOPE)

ExternalProject_Add(assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
  GIT_TAG v4.1.0
  SOURCE_DIR ${VENDOR}/assimp
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS <BINARY_DIR>/code/libassimp.dylib)
ExternalProject_Get_Property(assimp SOURCE_DIR BINARY_DIR)
set(ASSIMP_INCLUDE ${SOURCE_DIR}/include ${BINARY_DIR}/include PARENT_SCOPE)
set(ASSIMP_LIB ${BINARY_DIR}/code/libassimp.dylib PARENT_SCOPE)

ExternalProject_Add(anttweakbar
  URL https://downloads.sourceforge.net/project/anttweakbar/AntTweakBar_116.zip
  URL_HASH MD5=4349a13c0b5a493cf5b6b0757f3e7f0d
  SOURCE_DIR  ${VENDOR}/anttweakbar
  PATCH_COMMAND cp -R ${VENDOR}/patch/anttweakbar/ .
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make -C src -f Makefile.osx
  BUILD_IN_SOURCE true
  INSTALL_COMMAND ""
  BUILD_BYPRODUCTS <BINARY_DIR>/lib/libAntTweakBar.a)
ExternalProject_Get_Property(anttweakbar SOURCE_DIR BINARY_DIR)
set(ANTTWEAKBAR_INCLUDE ${SOURCE_DIR}/include PARENT_SCOPE)
set(ANTTWEAKBAR_LIB ${BINARY_DIR}/lib/libAntTweakBar.a PARENT_SCOPE)
