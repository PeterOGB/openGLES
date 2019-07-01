cmake_minimum_required(VERSION 2.8.0)
PROJECT(toolkit C)



SET(CMAKE_C_FLAGS "-g -Wall")

SET(CMAKE_C_FLAGS "-std=gnu99 -g  -Wall -Wextra -Wunused -Wconversion -Wundef " 
"-Wcast-qual -Wmissing-prototypes "
"-Wredundant-decls -Wunreachable-code -Wwrite-strings -Warray-bounds "
"-Wstrict-aliasing=3 -Wstrict-overflow=1 -Wstrict-prototypes -Winline "
"-Wshadow -Wswitch -Wmissing-include-dirs -Woverlength-strings -Wpacked "
"-Wdisabled-optimization  -Wformat=2 -Winit-self "
"-Wunused-parameter -Wlogical-op -Wuninitialized "
"-Wnested-externs -Wpointer-arith -Wdouble-promotion -Wunused-macros "
"-Wunsafe-loop-optimizations -rdynamic ")

STRING(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

find_package( PkgConfig REQUIRED)

pkg_check_modules ( GTK REQUIRED gtk+-3.0 )
include_directories(${GTK_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GTK_LIBRARIES})

ADD_EXECUTABLE(Ex1 Ex1.c toolkit.c toolkit.h)

target_link_libraries(Ex1 ${LIBS} GLESv2 EGL m )

ADD_EXECUTABLE(Ex2 Ex2.c toolkit.c toolkit.h)

target_link_libraries(Ex2 ${LIBS} GLESv2 EGL m )

ADD_EXECUTABLE(Ex3 Ex3.c toolkit.c toolkit.h)

target_link_libraries(Ex3 ${LIBS} GLESv2 EGL m )

ADD_EXECUTABLE(Ex4 Ex4.c toolkit.c toolkit.h)

target_link_libraries(Ex4 ${LIBS} GLESv2 EGL m )

ADD_EXECUTABLE(Ex5 Ex5.c toolkit.c toolkit.h)

target_link_libraries(Ex5 ${LIBS} GLESv2 EGL png16 m )
