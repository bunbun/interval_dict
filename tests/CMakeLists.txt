project(test_interval_dict LANGUAGES CXX DESCRIPTION "Boost test of interval_dict library.")

include_directories (${Boost_INCLUDE_DIRS})

# 'test_boost_intervaldict' is the target name
add_executable (test_intervaldict
                print_set.h
                print_tuple.h
                print_vector.h
                test_data.h
                test_icl.h
                test_icl_disjoint_intervals.cpp
                test_icl_erase.cpp
                test_icl_fill.cpp
                test_icl_find.cpp
                test_icl_flatten.cpp
                test_icl_insert.cpp
                test_icl_intervals.cpp
                test_icl_join.cpp
                test_icl_member_functions.cpp
                test_icl_merge_subtract.cpp
                test_icl_subset.cpp
                test_intervaldict.cpp test_data2.h)
target_link_libraries (test_intervaldict  PRIVATE interval_dict)

target_include_directories(test_intervaldict
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