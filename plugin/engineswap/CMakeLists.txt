cmake_minimum_required(VERSION 3.4)

set(engineswap_SOURCES "engineswap.cc aioengine.cc iouengine.cc posixengine.cc filedummies.cc" PARENT_SCOPE)
set(engineswap_INCLUDE_PATHS "${LIBCRYPTO_HOME}/include" PARENT_SCOPE)
set(engineswap_LINK_PATHS "${LIBCRYPTO_HOME}/lib" PARENT_SCOPE)
if(MSVC)
  set(engineswap_LIBS "libcrypto" PARENT_SCOPE)
  set(engineswap_CMAKE_SHARED_LINKER_FLAGS "/EXPORT:engineswap_reg" PARENT_SCOPE)
  set(engineswap_CMAKE_EXE_LINKER_FLAGS "/INCLUDE:engineswap_reg" PARENT_SCOPE)
else()
  set(engineswap_LIBS "crypto" PARENT_SCOPE)
  set(engineswap_CMAKE_EXE_LINKER_FLAGS "-u engineswap_reg" PARENT_SCOPE)
endif()
