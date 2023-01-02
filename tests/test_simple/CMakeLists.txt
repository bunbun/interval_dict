project(test_interval_dict LANGUAGES CXX DESCRIPTION "Boost test of interval_dict library.")

set(TARGET_NAME test_simple)
#set(CMAKE_BUILD_TYPE "coverage")
include_directories (${Boost_INCLUDE_DIRS})


# 'test_boost_intervaldict' is the target name
add_executable (${TARGET_NAME}
        ../test_intervaldict.cpp
        ../test_simple.cpp
        )

set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
target_link_libraries (${TARGET_NAME}  PRIVATE interval_dict)
target_compile_definitions(${TARGET_NAME} PRIVATE INTERVALDICTTESTTYPE=IntervalDictITreeExp)
target_include_directories(${TARGET_NAME}
        PUBLIC
        # Used when building the library:
        $<BUILD_INTERFACE:${interval_dict_SOURCE_DIR}/include/interval_dict>
        )
if(CMAKE_BUILD_TYPE STREQUAL "coverage" OR CODE_COVERAGE)

    if("${CMAKE_C_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
        message("Building with llvm Code Coverage Tools")

        # Warning/Error messages
        #endif()

        # set Flags
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")

    elseif(CMAKE_COMPILER_IS_GNUCXX)
        message("Building with lcov Code Coverage Tools")

        # Warning/Error messages
        if(NOT (CMAKE_BUILD_TYPE STREQUAL "Debug"))
            message(WARNING "Code coverage results with an optimized (non-Debug) build may be misleading")
        endif()
        if(NOT LCOV_PATH)
            message(FATAL_ERROR "lcov not found! Aborting...")
        endif()
        if(NOT GENHTML_PATH)
            message(FATAL_ERROR "genhtml not found! Aborting...")
        endif()

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
    else()
        message(FATAL_ERROR "Code coverage requires Clang or GCC. Aborting.")
    endif()
endif()


# llvm-cov
add_custom_target(${TARGET_NAME}-ccov-preprocessing
        COMMAND LLVM_PROFILE_FILE=${TARGET_NAME}.profraw $<TARGET_FILE:${TARGET_NAME}>
        COMMAND llvm-profdata merge -sparse ${TARGET_NAME}.profraw -o ${TARGET_NAME}.profdata
        DEPENDS ${TARGET_NAME})

add_custom_target(${TARGET_NAME}-ccov-show
        COMMAND llvm-cov show $<TARGET_FILE:${TARGET_NAME}> -instr-profile=${TARGET_NAME}.profdata -show-line-counts-or-regions
        DEPENDS ${TARGET_NAME}-ccov-preprocessing)

add_custom_target(${TARGET_NAME}-ccov-report
        COMMAND llvm-cov report $<TARGET_FILE:${TARGET_NAME}> -instr-profile=${TARGET_NAME}.profdata
        DEPENDS ${TARGET_NAME}-ccov-preprocessing)

add_custom_target(${TARGET_NAME}-ccov
        COMMAND mkdir -p ${CMAKE_BINARY_DIR}/${TARGET_NAME}-llvm-cov
        COMMAND llvm-cov show $<TARGET_FILE:${TARGET_NAME}> -instr-profile=${TARGET_NAME}.profdata -show-line-counts-or-regions -output-dir=${CMAKE_BINARY_DIR}/${TARGET_NAME}-llvm-cov -format="html"
        DEPENDS ${TARGET_NAME}-ccov-preprocessing)

add_custom_command(TARGET ${TARGET_NAME}-ccov POST_BUILD
        COMMAND ;
        COMMENT "Open ${CMAKE_BINARY_DIR}/${TARGET_NAME}-llvm-cov/index.html in your browser to view the coverage report."
        )

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
