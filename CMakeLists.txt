cmake_minimum_required(VERSION 3.20)
project(gof23 CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Wpedantic)

add_compile_options(-fno-exceptions)

include(CTest)

find_package(absl CONFIG REQUIRED)

if (BUILD_TESTING)
    find_package(GTest CONFIG REQUIRED)
    include(GoogleTest)
endif (BUILD_TESTING)


file(GLOB CHILD_DIRS RELATIVE "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/src/*")

set(ALL_EXES)
foreach (child ${CHILD_DIRS})
    set(dir "${CMAKE_SOURCE_DIR}/src/${child}")
    if (NOT IS_DIRECTORY "${dir}")
        continue()
    endif ()

    file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "${dir}/*.cpp")
    list(FILTER SRCS EXCLUDE REGEX ".*/tests/.*\\.test\\.cpp$")

    if (EXISTS "${dir}/main.cpp")
        add_executable(${child} ${SRCS})
        target_include_directories(${child} PRIVATE "${dir}")
        target_link_libraries(${child} PRIVATE absl::strings absl::str_format absl::status absl::statusor absl::hash absl::flat_hash_map absl::raw_hash_set
        )
        list(APPEND ALL_EXES ${child})
    endif ()

    if (BUILD_TESTING)
        file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS "${dir}/tests/*.test.cpp")
        if (TESTS)
            set(NOMAIN_SRCS ${SRCS})
            list(FILTER NOMAIN_SRCS EXCLUDE REGEX ".*/main\\.cpp$")
            add_executable(${child}_test ${NOMAIN_SRCS} ${TESTS})
            target_include_directories(${child}_test PRIVATE "${dir}")
            target_link_libraries(${child}_test PRIVATE GTest::gtest_main GTest::gmock absl::strings absl::str_format absl::status absl::statusor absl::hash absl::flat_hash_map absl::raw_hash_set
            )
            gtest_discover_tests(${child}_test
                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                    DISCOVERY_TIMEOUT 60)
        endif ()
    endif ()
endforeach ()

if (ALL_EXES)
    add_custom_target(all_patterns DEPENDS ${ALL_EXES})
endif ()
