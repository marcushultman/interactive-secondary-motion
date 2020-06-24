include(ExternalProject)

set_property(DIRECTORY PROPERTY EP_PREFIX ${CMAKE_BINARY_DIR}/vendor)

set(VENDOR ${CMAKE_CURRENT_SOURCE_DIR})

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
