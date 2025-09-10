cmake_minimum_required(VERSION 3.20)
project(gof23 CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

include(CTest)

file(GLOB CHILD_DIRS RELATIVE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/*")

set(ALL_EXES)
foreach(child ${CHILD_DIRS})
    set(dir "${CMAKE_SOURCE_DIR}/src/${child}")
    if(NOT IS_DIRECTORY "${dir}")
        continue()
    endif()

    file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "${dir}/*.cpp")
    list(FILTER SRCS EXCLUDE REGEX ".*/tests/.*_test\\.cpp$")

    if(EXISTS "${dir}/main.cpp")
        add_executable(${child} ${SRCS}
                src/01_singleton/main.cpp)
        target_include_directories(${child} PRIVATE "${dir}")
        list(APPEND ALL_EXES ${child})
    endif()

    if(BUILD_TESTING)
        file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS "${dir}/tests/*_test.cpp")
        if(TESTS)
            set(NOMAIN_SRCS ${SRCS})
            list(FILTER NOMAIN_SRCS EXCLUDE REGEX ".*/main\\.cpp$")
            add_executable(${child}_test ${NOMAIN_SRCS} ${TESTS})
            target_include_directories(${child}_test PRIVATE "${dir}")
            target_link_libraries(${child}_test PRIVATE GTest::gtest_main)
            add_test(NAME ${child}_test COMMAND ${child}_test)
        endif()
    endif()
endforeach()

if(ALL_EXES)
    add_custom_target(all_patterns DEPENDS ${ALL_EXES})
endif()
