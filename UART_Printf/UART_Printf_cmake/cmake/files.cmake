# file-format: 1.0.0
if(CMAKE_BUILD_TYPE STREQUAL "debug_GCC_NUCLEO-C562RE")
  target_sources(${CMAKE_PROJECT_NAME} PRIVATE main.c main.h)
endif()
