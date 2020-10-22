project(test_interval_dict LANGUAGES CXX DESCRIPTION "Boost test of interval_dict library.")

set(TARGET_NAME test_general)

include_directories (${Boost_INCLUDE_DIRS})

# 'test_boost_intervaldict' is the target name
add_executable (${TARGET_NAME}
        ../test_intervaldict.cpp
        ../test_interval_overlaps.cpp)

set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
target_link_libraries (${TARGET_NAME}  PRIVATE interval_dict)
target_include_directories(${TARGET_NAME}
        PUBLIC
        # Used when building the library:
        $<BUILD_INTERFACE:${interval_dict_SOURCE_DIR}/include/interval_dict>
        )

# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()
#