find_program(DOXYGEN doxygen)
if (NOT DOXYGEN)
  message(STATUS "Target 'doc' disabled (requires doxygen)")
  return ()
endif ()

find_package(PythonInterp QUIET REQUIRED)
find_program(SPHINX_EXECUTABLE NAMES sphinx-build sphinx-build-3)

set(SPHINX_EXECUTABLE ${SPHINX_EXECUTABLE})

message(STATUS "${PYTHON_EXECUTABLE} ${SPHINX_EXECUTABLE} ${INTERVAL_DICT_VER} SOURCES index.rst api.rst usage.rst design.rst build.py conf.py")
add_custom_target(doc
  COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/build.py
                               ${SPHINX_EXECUTABLE} ${INTERVAL_DICT_VER}
        #        @$(SPHINXBUILD) -M help "$(SOURCEDIR)" "$(BUILDDIR)" $(SPHINXOPTS) $(O)

  SOURCES contents.rst index.rst api.rst usage.rst design.rst build.py conf.py)
# _templates/layout.html

include(GNUInstallDirs)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/doc/interval_dict OPTIONAL
        PATTERN ".doctrees" EXCLUDE)

