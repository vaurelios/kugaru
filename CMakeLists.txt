project(kugaru)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(PkgConfig)


# Deal with dependencies...
pkg_check_modules(GL REQUIRED gl)
pkg_check_modules(GLU REQUIRED glu)
pkg_check_modules(AL REQUIRED openal)
pkg_check_modules(SDL REQUIRED sdl)
pkg_check_modules(PNG REQUIRED libpng)
pkg_check_modules(ZLIB REQUIRED zlib)

include_directories(
    ${GL_INCLUDE_DIRS}
    ${GLU_INCLUDE_DIRS}
    ${AL_INCLUDE_DIRS}
    ${SDL_INCLUDE_DIRS}
    ${PNG_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)

set(KUGARU_LIBS
    ${GL_LIBRARIES}
    ${GLU_LIBRARIES}
    ${AL_LIBRARIES}
    ${SDL_LIBRARIES}
    ${PNG_LIBRARIES}
    ${ZLIB_LIBRARIES}
)

subdirs(src)
