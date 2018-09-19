set(CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.1.2)
cmake_policy(SET CMP0015 NEW) # Link Directory Pathing
set(CMAKE_DEBUG_POSTFIX _d)

if (APPLE)  
  set(CMAKE_POSITION_INDEPENDENT_CODE False)
else()
  set(CMAKE_POSITION_INDEPENDENT_CODE True)
endif()

set(BUILD_TESTING NO CACHE BOOL "Enable/Disable testing")
set(IS_PYTHON_BUILD NO CACHE BOOL "Enable/Disable PYTHON")
set(IS_CONDA_BUILD NO CACHE BOOL "Set this if you want to make a conda package.")
set(PYTHON_TARGET_VERSION 3.6 CACHE STRING "Version of python to link to for python wrapping.")

if(IS_PYTHON_BUILD AND BUILD_TESTING)
  message(FATAL_ERROR "Cannot build python module when testing is enabled")
endif()


project(xmsgeom C CXX)

if(WIN32)
    if(XMS_BUILD)
        add_definitions(/D _WIN32_WINNT=0x0501)  # Windows XP and higher
        add_definitions(/Zc:wchar_t-)  # Treat wchar_t as built-in type
    else(NOT XMS_BUILD)
        add_definitions(/D BOOST_ALL_NO_LIB)
    endif()
endif()

IF(IS_CONDA_BUILD)
  include(${CMAKE_CURRENT_LIST_DIR}/condabuildinfo.cmake)

ELSE() # If we are not using conda, we are using conan
  # C++11 Support

  # Conan setup
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup(TARGETS)
  set(EXT_INCLUDE_DIRS ${CONAN_INCLUDE_DIRS})
  set(EXT_LIB_DIRS ${CONAN_LIB_DIRS})
  set(EXT_LIBS ${CONAN_LIBS})
endif(IS_CONDA_BUILD)

message("External Include Dirs: ${EXT_INCLUDE_DIRS}")
message("External Lib Dirs: ${EXT_LIB_DIRS}")
message("Extneral Libs: ${EXT_LIBS}")

if(IS_PYTHON_BUILD)
  # linux and mac builds for conan (on TRAVISCI) use a docker that has python
  # 2.7 as system python. We do not have control over that docker image so we
  # can't change this and it is fine for building conan packages and checking
  # that we don't have errors in the python wrapping. We have conda recipes
  # for building python packages that target other versions of python.
  find_package(PythonLibs ${PYTHON_TARGET_VERSION} EXACT REQUIRED)
  list(APPEND EXT_INCLUDE_DIRS
    ${PYTHON_INCLUDE_DIRS}
  )
  # Pybind11 module
  if(IS_CONDA_BUILD)
      include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11Targets.cmake")
      include("${CONDA_PREFIX}/share/cmake/pybind11/FindPythonLibsNew.cmake")
      include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11Config.cmake")
      include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11ConfigVersion.cmake")
      include("${CONDA_PREFIX}/share/cmake/pybind11/pybind11Tools.cmake")
  else()
      include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11Targets.cmake")
      include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/FindPythonLibsNew.cmake")
      include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11Config.cmake")
      include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11ConfigVersion.cmake")
      include("${CONAN_PYBIND11_ROOT}/share/cmake/pybind11/pybind11Tools.cmake")
  endif()
  list(APPEND EXT_INCLUDE_DIRS
    ${PYTHON_INCLUDE_DIRS}
  )
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${EXT_INCLUDE_DIRS})
link_directories(${EXT_LIB_DIRS})

# Sources
set(xmsgeom_sources
  xmsgeom/geometry/geoms.cpp
  xmsgeom/geometry/GmExtents.cpp
  xmsgeom/geometry/GmMultiPolyIntersectionSorterTerse.cpp
  xmsgeom/geometry/GmMultiPolyIntersector.cpp
  xmsgeom/geometry/GmPolygon.cpp
  xmsgeom/geometry/GmPolyLinePtRedistributer.cpp
  xmsgeom/geometry/GmPtSearch.cpp
  xmsgeom/geometry/GmTriSearch.cpp
  xmsgeom/triangulate/TrBreaklineAdder.cpp
  xmsgeom/triangulate/triangles.cpp
  xmsgeom/triangulate/TrTin.cpp
  xmsgeom/triangulate/TrTriangulator.cpp
  xmsgeom/triangulate/TrTriangulatorPoints.cpp
  xmsgeom/triangulate/detail/TrAutoFixFourTrianglePts.cpp
  xmsgeom/triangulate/detail/triangulate.cpp
  xmsgeom/triangulate/detail/TrOuterTriangleDeleter.cpp
)

set(xmsgeom_headers
  xmsgeom/geometry/geoms.h
  xmsgeom/geometry/GmBoostTypes.h
  xmsgeom/geometry/GmExtents.h
  xmsgeom/geometry/GmMultiPolyIntersectionSorter.h
  xmsgeom/geometry/GmMultiPolyIntersectionSorterTerse.h
  xmsgeom/geometry/GmMultiPolyIntersector.h
  xmsgeom/geometry/GmMultiPolyIntersectorData.h
  xmsgeom/geometry/GmPolygon.h
  xmsgeom/geometry/GmPolyLinePtRedistributer.h
  xmsgeom/geometry/GmPtSearch.h
  xmsgeom/geometry/GmTriSearch.h
  xmsgeom/triangulate/TrBreaklineAdder.h
  xmsgeom/triangulate/triangles.h
  xmsgeom/triangulate/TrTin.h
  xmsgeom/triangulate/TrTriangulator.h
  xmsgeom/triangulate/TrTriangulatorPoints.h
  xmsgeom/triangulate/detail/TrAutoFixFourTrianglePts.h
  xmsgeom/triangulate/detail/triangulate.h
  xmsgeom/triangulate/detail/TrOuterTriangleDeleter.h
)

# Tests
if (BUILD_TESTING)
  add_definitions(-DCXX_TEST -DCXXTEST4)

  list(APPEND xmsgeom_sources
    xmsgeom/geometry/geoms.t.h
    xmsgeom/geometry/GmExtents.t.h
    xmsgeom/geometry/GmMultiPolyIntersector.t.h
    xmsgeom/geometry/GmPolygon.t.h
    xmsgeom/geometry/GmPolyLinePtRedistributer.t.h
    xmsgeom/geometry/GmPtSearch.t.h
    xmsgeom/geometry/GmTriSearch.t.h
    xmsgeom/triangulate/TrBreaklineAdder.h
    xmsgeom/triangulate/TrBreaklineAdder.t.h
    xmsgeom/triangulate/TrTin.t.h
    xmsgeom/triangulate/TrTriangulatorPoints.t.h
    xmsgeom/triangulate/detail/TrAutoFixFourTrianglePts.t.h
    xmsgeom/triangulate/detail/TrOuterTriangleDeleter.t.h
  )

  find_package(CxxTest)
  if(CXXTEST_FOUND)
    include_directories(${CXXTEST_INCLUDE_DIRS})
    enable_testing()

    set(CXXTEST_TESTGEN_ARGS --xunit-printer)
    file(GLOB_RECURSE test_headers ${CMAKE_CURRENT_LIST_DIR}/xmsgeom/*.t.h)
    CXXTEST_ADD_TEST(
      runner runner.cpp ${test_headers}
    )
    target_link_libraries(runner ${PROJECT_NAME})

    if(WIN32)
      set(RUNNERNAME "runner.exe")
    else()
      set(RUNNERNAME "runner")
    endif()
  endif()
endif()

# Static library
add_library(${PROJECT_NAME} STATIC
  ${xmsgeom_sources} ${xmsgeom_headers}
)
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME}
  ${EXT_LIBS}
)
target_link_libraries(${PROJECT_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
)
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} rt)
endif()

if(IS_PYTHON_BUILD)

  # Pybind11 sources
  set(xmsgeom_py
    xmsgeom/python/triangulate/triangulate_py.cpp
    xmsgeom/python/triangulate/TrTin_py.cpp
    xmsgeom/python/triangulate/TrTriangulatorPoints_py.cpp
    xmsgeom/python/xmsgeom_py.cpp
  )

  set(xmsgeom_py_headers
    xmsgeom/python/triangulate/triangulate_py.h
  )

  message(STATUS "PYTHON_INCLUDE: ${PYTHON_INCLUDE_DIRS}")
  message(STATUS "PYTHON_LIBS: ${PYTHON_LIBRARIES}")

  pybind11_add_module(xmsgeom_py
    ${xmsgeom_py} ${xmsgeom_py_headers}
  )
  target_include_directories(xmsgeom_py
      PRIVATE
        ${EXT_INCLUDE_DIRS}
  )
  target_link_libraries(xmsgeom_py
    PRIVATE
      ${EXT_LIBS}
      ${PROJECT_NAME}
  )
  set_target_properties(xmsgeom_py PROPERTIES
    LINKER_LANGUAGE CXX
  )

  # Install recipe
  install(
    TARGETS xmsgeom_py
    ARCHIVE DESTINATION "site-packages"
    LIBRARY DESTINATION "site-packages"
  )
endif()

# Install recipe
install(
  TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION "lib"
  LIBRARY DESTINATION "lib"
)
foreach (header IN LISTS xmsgeom_headers xmsgeom_py_headers)
  get_filename_component(subdir "${header}" DIRECTORY)
  install(
    FILES "${header}"
    DESTINATION "include/${subdir}"
  )
endforeach ()
