cmake_minimum_required(VERSION 3.5)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

project(npdfr)

option(NPDFR_DEPLOYMENT "Compile with full optimizations." ON)

add_definitions("-Wall -Wimplicit-fallthrough -std=c++20 -DNPDFR_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

if (NPDFR_DEPLOYMENT)
    add_definitions("-O3")
else ()
    add_definitions("-g")
endif ()

file(GLOB NPDFR_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(npdfr ${NPDFR_SOURCES})
set_target_properties(npdfr PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_custom_target(venv COMMAND python -m venv "${PROJECT_SOURCE_DIR}/venv" && source "${PROJECT_SOURCE_DIR}/venv/bin/activate" && pip install -r "${PROJECT_SOURCE_DIR}/requirements.txt")
add_dependencies(npdfr venv)

add_custom_target(launcher COMMAND "${PROJECT_SOURCE_DIR}/sys/create_launcher.sh" "${CMAKE_INSTALL_PREFIX}" "${PROJECT_SOURCE_DIR}/bin/launcher")
add_dependencies(npdfr launcher)

add_custom_target(manual COMMAND mkdir -p "${PROJECT_SOURCE_DIR}/sys/share/man/man1" && gzip -c "${PROJECT_SOURCE_DIR}/sys/npdfr.1" > "${PROJECT_SOURCE_DIR}/sys/share/man/man1/npdfr.1.gz")
add_dependencies(npdfr manual)

target_link_libraries(npdfr jsoncpp)
target_link_libraries(npdfr ncursesw)

install(FILES "${PROJECT_SOURCE_DIR}/bin/launcher" DESTINATION bin RENAME npdfr PERMISSIONS
    OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
install(TARGETS npdfr DESTINATION share/npdfr)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/venv" DESTINATION share/npdfr)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/tools" DESTINATION share/npdfr)
install(DIRECTORY "${PROJECT_SOURCE_DIR}/sys/share/" DESTINATION share)
