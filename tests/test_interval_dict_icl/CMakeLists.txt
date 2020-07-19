project(test_interval_dict LANGUAGES CXX DESCRIPTION "Boost test of interval_dict library.")

include_directories (${Boost_INCLUDE_DIRS})

# 'test_boost_intervaldict' is the target name
add_executable (test_interval_dict_icl
        ../print_set.h
        ../print_tuple.h
        ../print_vector.h
        ../test_data.h
        ../test_data2.h
        ../test_disjoint_intervals.cpp
        ../test_erase.cpp
        ../test_fill.cpp
        ../test_find.cpp
        ../test_flatten.cpp
        ../test_insert.cpp
        ../test_intervals.cpp
        ../test_join.cpp
        ../test_member_functions.cpp
        ../test_merge_subtract.cpp
        ../test_subset.cpp
        ../test_utils.h
        ../test_intervaldict.cpp
        )
target_link_libraries (test_interval_dict_icl  PRIVATE interval_dict)
target_compile_definitions(test_interval_dict_icl PRIVATE INTERVALDICTTESTTYPE=IntervalDictICLExp)
target_include_directories(test_interval_dict_icl
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