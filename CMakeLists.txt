cmake_minimum_required(VERSION 3.22)

project(
    Loxt
    VERSION 0.1
    DESCRIPTION "Loxt programming language"
    LANGUAGES CXX
)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release)
    endif()

    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Og")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")

    include(CTest)

    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
    else()
        message(WARNING "Doxygen not found")
    endif()
endif()

include(FetchContent)

find_program(
    CLANG_TIDY_EXE
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
)

if(CLANG_TIDY_EXE)
    message(STATUS "clang-tidy found at ${CLANG_TIDY_EXE}")
else()
    message(WARNING "clang-tidy not found.")
endif()


set(
    DO_CLANG_TIDY
    "${CLANG_TIDY_EXE};--config-file=${Loxt_SOURCE_DIR}/.clang-tidy;--header-filter=${Loxt_SOURCE_DIR}/include/.*"
)


add_subdirectory(src)
add_subdirectory(driver)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR LOXT_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    add_subdirectory(test)
endif()
