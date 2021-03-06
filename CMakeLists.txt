cmake_minimum_required(VERSION 2.8)


if(DEFINED SLP)
else()
  set(SLP "0")
endif()


find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "  sleep ${SLP};  CCACHE_DIR=/tmp/ccachedir ccache")
endif(CCACHE_FOUND)


set(CMAKE_VERBOSE_MAKEFILE ON)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()


set(CMAKE_C_COMPILER  gcc)
set(CMAKE_C_FLAGS   "-std=c14 -Werror")
set(CMAKE_CXX_COMPILER  "g++")
set(CMAKE_CXX_FLAGS "-std=c++14 -Werror")
set(CMAKE_CXX_LINK_FLAGS  "-lstdc++ -lz -lpng -lSDL2 -lSDL2_image -lSDL2main")


set(CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS}   -O2 -march=core2 -fomit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -march=core2 -fomit-frame-pointer")
set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS}   -O0 -g")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -O0 -g")


if(EM)
  set(EXTENSION ".html")
  set(CMAKE_CXX_COMPILER  em++)
  set(CMAKE_CXX_FLAGS "-std=c++14 -Werror -Wno-switch -O2")
  set(CMAKE_CXX_FLAGS_DEBUG "")
  set(CMAKE_CXX_LINK_FLAGS  "${CMAKE_CXX_LINK_FLAGS} -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0 --preload-file texture.png --use-preload-plugins")
endif()




set(SOURCES
  libjson/json_stream.cpp
  libjson/json_stream__scan.cpp
  libjson/json_value.cpp
  libjson/json_FileBuffer.cpp
  isom_LineContext.cpp
  isom_point.cpp
  isom_transformer.cpp
  isom_dot.cpp
  isom_line.cpp
  isom_polygon.cpp
  isom_tetragon.cpp
  isom_object.cpp
  isom_renderer.cpp
  isom_renderer__draw.cpp
  isom_math.cpp
  isom_font.cpp
  isom_image.cpp
  isom_screen.cpp
  isom_load_object.cpp
  isom_load_stage.cpp
)


set(EXPREE_SOURCES
  expree/expree_element.cpp
  expree/expree_element__make_value.cpp
  expree/expree_operand.cpp
  expree/expree_scope.cpp
  expree/expree_primitive.cpp
  expree/expree_object.cpp
  expree/expree_object__operate.cpp
  expree/expree_MemorySpace.cpp
  expree/expree_parser.cpp
  expree/expree_parser__make_element.cpp
)


include_directories(.)
include_directories(./expree)
include_directories(/usr/include/SDL2)
add_executable("testLineContext${EXTENSION}" testLineContext.cpp ${SOURCES} ${EXPREE_SOURCES})
add_executable("testBuilder${EXTENSION}" testBuilder.cpp ${SOURCES} ${EXPREE_SOURCES})
add_executable("testCoBuilder${EXTENSION}" testCoBuilder.cpp ${SOURCES} ${EXPREE_SOURCES})


if(EM)
  set_target_properties("testBuilder${EXTENSION}" PROPERTIES LINK_FLAGS "--shell-file ../shell__tB.html")
  set_target_properties("testCoBuilder${EXTENSION}" PROPERTIES LINK_FLAGS "--shell-file ../shell__tCB.html")
  set_target_properties("testLineContext${EXTENSION}" PROPERTIES LINK_FLAGS "--shell-file ../shell__tLC.html")
endif()




