project(simple LANGUAGES CXX DESCRIPTION "Simple Example.")

function(interval_dict_executable)
    cmake_parse_arguments(
            IDICTEXEC
            ""
            "TARGET_NAME"
            "FILES"
            ${ARGN}
    )
    set(TARGET_NAME ${IDICTEXEC_TARGET_NAME})
    add_executable (${TARGET_NAME} ${IDICTEXEC_FILES})
    set_target_properties(${TARGET_NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
            )
    target_link_libraries (${TARGET_NAME}  PRIVATE interval_dict)
    target_include_directories(${TARGET_NAME}
            PUBLIC
            # Used when building the library:
            $<BUILD_INTERFACE:${interval_dict_SOURCE_DIR}/include/interval_dict>
            )
endfunction()

interval_dict_executable(TARGET_NAME simple FILES simple.cpp)
interval_dict_executable(TARGET_NAME simple_exp FILES simple_exp.cpp)
interval_dict_executable(TARGET_NAME simple_icl FILES simple_icl.cpp)
interval_dict_executable(TARGET_NAME simple_iclexp FILES simple_iclexp.cpp)
interval_dict_executable(TARGET_NAME simple_bi FILES simple_bi.cpp)
interval_dict_executable(TARGET_NAME simple_bi_exp FILES simple_bi_exp.cpp)
interval_dict_executable(TARGET_NAME simple_bi_icl FILES simple_bi_icl.cpp)
interval_dict_executable(TARGET_NAME simple_bi_iclexp FILES simple_bi_iclexp.cpp)