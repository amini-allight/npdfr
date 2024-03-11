cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(npdfr)

option(NPDFR_DEPLOYMENT "Compile with full optimizations." ON)

add_definitions("-Wall -Wimplicit-fallthrough -std=c++20")

if (NPDFR_DEPLOYMENT)
    add_definitions("-O3")
else ()
    add_definitions("-g")
endif ()

file(GLOB NPDFR_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(npdfr ${NPDFR_SOURCES})
set_target_properties(npdfr PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

target_link_libraries(npdfr jsoncpp)
target_link_libraries(npdfr ncursesw)
