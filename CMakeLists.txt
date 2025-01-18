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

add_custom_target(manual COMMAND mkdir -p "${PROJECT_SOURCE_DIR}/sys/share/man/man1" && gzip -c "${PROJECT_SOURCE_DIR}/sys/npdfr.1" > "${PROJECT_SOURCE_DIR}/sys/share/man/man1/npdfr.1.gz")
add_dependencies(npdfr manual)

target_link_libraries(npdfr libmupdf.so)
target_link_libraries(npdfr ncursesw)

install(TARGETS npdfr DESTINATION bin)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/sys/share/" DESTINATION share)
