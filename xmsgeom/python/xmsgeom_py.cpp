//------------------------------------------------------------------------------
/// \file
/// \brief root module for xmsgeom Python bindings.
/// \copyright (C) Copyright Aquaveo 2018. Distributed under the xmsng
///  Software License, Version 1.0. (See accompanying file
///  LICENSE_1_0.txt or copy at http://www.aquaveo.com/xmsng/LICENSE_1_0.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------
#include <pybind11/pybind11.h>
#include <xmsgeom/python/triangulate/triangulate_py.h>

//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------
std::string version() {
    return "1.0.0";
}


//------ Primary Module --------------------------------------------------------
PYBIND11_MODULE(xmsgeom_py, m) {
    m.doc() = "Python bindings for xmsgeom"; // optional module docstring
    m.def("version", &version,
          "Get current version of xmsgeom Python bindings.");

    // UGrid module
    py::module modTriangulate = m.def_submodule("triangulate");
    initTriangulate(modTriangulate);
}

