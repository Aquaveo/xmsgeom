//------------------------------------------------------------------------------
/// \file
/// \brief
/// \copyright (C) Copyright Aquaveo 2018. Distributed under the xmsng
///  Software License, Version 1.0. (See accompanying file
///  LICENSE_1_0.txt or copy at http://www.aquaveo.com/xmsng/LICENSE_1_0.txt)
//------------------------------------------------------------------------------

//----- Included files ---------------------------------------------------------
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <boost/shared_ptr.hpp>
#include <xmscore/python/misc/PyUtils.h>
#include <xmsgeom/triangulate/TrTin.h>
#include <iostream>
#include <fstream>


//----- Namespace declaration --------------------------------------------------
namespace py = pybind11;

//----- Python Interface -------------------------------------------------------
PYBIND11_DECLARE_HOLDER_TYPE(T, boost::shared_ptr<T>);

void initTrTin(py::module &m) {

      // Class
  py::class_<xms::TrTin, boost::shared_ptr<xms::TrTin>> iTrTin(m, "TrTin");

  iTrTin.def(py::init(&xms::TrTin::New));

  // ---------------------------------------------------------------------------
  // function: set_points
  // ---------------------------------------------------------------------------
  const char* set_points_doc = R"pydoc(
      Sets the tin points.

      Args:
          vec_pts (iterable): The points.
  )pydoc";
  iTrTin.def("set_points", [](xms::TrTin &self, py::iterable pts) {
          BSHP<xms::VecPt3d> vec_pts = xms::VecPt3dFromPyIter(pts);
          self.SetPoints(vec_pts);
      },set_points_doc, py::arg("vec_pts"));
  // ---------------------------------------------------------------------------
  // function: set_triangles
  // ---------------------------------------------------------------------------
  const char* set_triangles_doc = R"pydoc(
      Sets the tin triangles.

      Args:
          vec_tris_adj (iterable):  0-based indices of each triangle's points 
            (grouped by 3s)
  )pydoc";
  iTrTin.def("set_triangles", [](xms::TrTin &self, py::iterable tris) {
          BSHP<xms::VecInt> vec_tris = xms::VecIntFromPyIter(tris);
          self.SetTriangles(vec_tris);
      },set_triangles_doc,py::arg("vec_tris_adj"));
  // ---------------------------------------------------------------------------
  // function: set_triangles_adjacent_to_points
  // ---------------------------------------------------------------------------
  const char* set_triangles_adjacent_to_points_doc = R"pydoc(
      Sets the adjacency info of triangles adjacent to points.

      Args:
          vec_tris_adj (iterable): 0-based indices of triangles adjacent to 
            points.
  )pydoc";
  iTrTin.def("set_triangles_adjacent_to_points", [](xms::TrTin &self, py::iterable tris_adj) {
          BSHP<xms::VecInt2d> vec_tris_adj = xms::VecInt2dFromPyIter(tris_adj);
          self.SetTrianglesAdjacentToPoints(vec_tris_adj);
      },set_triangles_adjacent_to_points_doc,py::arg("vec_tris_adj"));
  // ---------------------------------------------------------------------------
  // function: set_geometry
  // ---------------------------------------------------------------------------
  const char* set_geometry_doc = R"pydoc(
      Set all the tin geometry at once (points, triangles, adjacency).

      Args:
          vec_pts (iterable): The tin points.
          vec_tris (iterable): 0-based indices of triangle points 
            (grouped by 3s)
          vec_tris_adj (iterable): 0-based indices of triangles adjacent to 
            points.
  )pydoc";
  iTrTin.def("set_geometry", [](xms::TrTin &self, py::iterable pts, py::iterable tris, py::iterable tris_adj) {
          boost::shared_ptr<xms::VecPt3d> vec_pts = xms::VecPt3dFromPyIter(pts);
          boost::shared_ptr<xms::VecInt> vec_tris = xms::VecIntFromPyIter(tris);
          boost::shared_ptr<xms::VecInt2d> vec_tris_adj = xms::VecInt2dFromPyIter(tris_adj);
          self.SetGeometry(vec_pts, vec_tris, vec_tris_adj);
      },set_geometry_doc, py::arg("vec_pts"), py::arg("vec_tris"), 
        py::arg("vec_tris_adj"));
  // ---------------------------------------------------------------------------
  // function: pts
  // ---------------------------------------------------------------------------
  const char* pts_doc = R"pydoc(
      Return the tin points.

      Returns:
          iterable: the tin points
  )pydoc";
  iTrTin.def_property_readonly("pts", [](xms::TrTin &self) -> py::iterable {
          xms::VecPt3d pts = self.Points();
          return xms::PyIterFromVecPt3d(pts);
      },pts_doc);
  // ---------------------------------------------------------------------------
  // function: tris
  // ---------------------------------------------------------------------------
  const char* tris_doc = R"pydoc(
      Return 0-based indices of triangle points (grouped by 3s).

      Returns:
          iterable: 0-based indices of triangle points (grouped by 3s).
  )pydoc";
  iTrTin.def_property_readonly("tris", [](xms::TrTin &self) -> py::iterable {
          xms::VecInt tris = self.Triangles();
          return xms::PyIterFromVecInt(tris);
      },tris_doc);
  // ---------------------------------------------------------------------------
  // function: tris_adj
  // ---------------------------------------------------------------------------
  const char* tris_adj_doc = R"pydoc(
      Returns triangles adjacent to points (0-based).

      Returns:
          iterable: triangles adjacent to points (0-based).
  )pydoc";
  iTrTin.def_property_readonly("tris_adj", 
    [](xms::TrTin &self) -> py::iterable {
          xms::VecInt2d tris_adj = self.TrisAdjToPts();
          return xms::PyIterFromVecInt2d(tris_adj);
      },tris_adj_doc);
  // ---------------------------------------------------------------------------
  // function: num_points
  // ---------------------------------------------------------------------------
  const char* num_points_doc = R"pydoc(
      Return the number of points.

      Returns:
          int: the number of points.
  )pydoc";
  iTrTin.def_property_readonly("num_points", &xms::TrTin::NumPoints,
    num_points_doc);
  // ---------------------------------------------------------------------------
  // function: num_triangles
  // ---------------------------------------------------------------------------
  const char* num_triangles_doc = R"pydoc(
      Return the number of triangles.

      Returns:
          int: the number of triangles.
  )pydoc";
  iTrTin.def_property_readonly("num_triangles", &xms::TrTin::NumTriangles,
    num_triangles_doc);
  // ---------------------------------------------------------------------------
  // function: triangle_from_edge
  // ---------------------------------------------------------------------------
  const char* triangle_from_edge_doc = R"pydoc(
      Finds the triangle with the edge defined by a_pt1 and a_pt2 and the local 
      index of those points. Compare to trTinTriFromEdge.

      Args:
        pt1 (int): First edge point index (0-based).
        pt2 (int): Second edge point index (0-based).

      Returns:
          tuple: Whether it was found, index of triangle or XM_NONE if not 
            found, Local pt (0-2) of pt1 in the tri, Local pt (0-2) of pt2 in 
            the tri.
  )pydoc";
  iTrTin.def("triangle_from_edge", 
    [](xms::TrTin &self, int pt1, int pt2) -> py::tuple {
          int tri, localPt1, localPt2;
          bool ret = self.TriangleFromEdge(pt1, pt2, tri, localPt1, localPt2);
          return py::make_tuple(ret, tri, localPt1, localPt2);
      },triangle_from_edge_doc, py::arg("pt1"), py::arg("pt2"));
  // ---------------------------------------------------------------------------
  // function: triangle_adjacent_to_edge
  // ---------------------------------------------------------------------------
  const char* triangle_adjacent_to_edge_doc = R"pydoc(
      Returns the triangle adjacent to the edge defined by a_pt1 and a_pt2. 
      Compare to trTriangleAdjacentToEdge.

      Args:
        pt1 (int): First edge point index (0-based).
        pt2 (int): Second edge point index (0-based).

      Returns:
          int: The triangle adjacent to the edge.
  )pydoc";
  iTrTin.def("triangle_adjacent_to_edge", [](xms::TrTin &self, int pt1, int pt2) -> int {
          return self.TriangleAdjacentToEdge(pt1, pt2);
      },triangle_adjacent_to_edge_doc, py::arg("pt1"), py::arg("pt2"));
  // ---------------------------------------------------------------------------
  // function: local_index
  // ---------------------------------------------------------------------------
  const char* local_index_doc = R"pydoc(
      Returns index (0-2) of point within triangle given global index. Compare 
      to trIndex.

      Args:
        tri (int): Triangle index (0-based).
        pt (int): Global point index (0-based).

      Returns:
          int: The local index (0-2) of the point pt in the triangle tri or 
            XM_NONE if pt is not on tri.
  )pydoc";
  iTrTin.def("local_index", &xms::TrTin::LocalIndex,local_index_doc,
    py::arg("tri"), py::arg("pt"));
  // ---------------------------------------------------------------------------
  // function: global_index
  // ---------------------------------------------------------------------------
  const char* global_index_doc = R"pydoc(
      Returns index into m_pts of a_localPt which is 0-2.

      Args:
        tri_idx (int): Triangle index (0-based).
        tri_idx (int): Local index (0-2) of point within the triangle.

      Returns:
          int: Global index into m_pts of localPt.
  )pydoc";
  iTrTin.def("global_index", &xms::TrTin::GlobalIndex,global_index_doc,
        py::arg("tri_idx"), py::arg("tri_idx"));
  // ---------------------------------------------------------------------------
  // function: vertices_are_adjacent
  // ---------------------------------------------------------------------------
  const char* vertices_are_adjacent_doc = R"pydoc(
      Return true if vertices form the edge of a triangle. Compare to 
      vrVerticesAreAdjacent.

      Args:
        pt1 (int): First point index (0-based).
        pt2 (int): Second point index (0-based).

      Returns:
          bool: true if points are adjacent.
  )pydoc";
  iTrTin.def("vertices_are_adjacent", &xms::TrTin::VerticesAreAdjacent,
    vertices_are_adjacent_doc, py::arg("pt1"),py::arg("pt2"));
  // ---------------------------------------------------------------------------
  // function: common_edge_index
  // ---------------------------------------------------------------------------
  const char* common_edge_index_doc = R"pydoc(
      Return index of common edge between triangle and neighbor. Edge index is 
      0-2 based on a_tri. Compare to trCommonEdgeIndex.

      Args:
        tri (int): Triangle index (0-based).
        adj_tri (int): Adjacent triangle index (0-based).

      Returns:
          int: Index of edge (0-2) in a_tri between a_tri and a_adTri, or 
            XM_NONE if not adjacent.
  )pydoc";
  iTrTin.def("common_edge_index", &xms::TrTin::CommonEdgeIndex,
    common_edge_index_doc, py::arg("tri"), py::arg("adj_tri"));
  // ---------------------------------------------------------------------------
  // function: adjacent_triangle
  // ---------------------------------------------------------------------------
  const char* adjacent_triangle_doc = R"pydoc(
      Returns the triangle adjacent to a_triIdx across a_edgeIdx (0-2). Compare 
      to trAdjacentTriangle.

      Args:
        tri_idx (int): Triangle index (0-based).
        edge_idx (int): Local edge index (0-2) in a_tri.

      Returns:
          int: Triangle adjacent to a_tri along it's a_edge or XM_NONE if there 
            is no triangle adjacent to the edge.
  )pydoc";
  iTrTin.def("adjacent_triangle", &xms::TrTin::AdjacentTriangle,
    adjacent_triangle_doc, py::arg("tri_idx"), py::arg("edge_idx"));
  // ---------------------------------------------------------------------------
  // function: triangle_centroid
  // ---------------------------------------------------------------------------
  const char* triangle_centroid_doc = R"pydoc(
      Calculate and return the centroid of a triangle.

      Args:
        tri (int): Triangle index (0-based).

      Returns:
          tuple: The centroid.
  )pydoc";
  iTrTin.def("triangle_centroid", [](xms::TrTin &self, int tri) -> py::tuple {
          xms::Pt3d pt = self.TriangleCentroid(tri);
          return xms::PyIterFromPt3d(pt);
      },triangle_centroid_doc,py::arg("tri"));
  // ---------------------------------------------------------------------------
  // function: triangle_area
  // ---------------------------------------------------------------------------
  const char* triangle_area_doc = R"pydoc(
      Calculate and return the area of a triangle.

      Args:
        tri (int): Triangle index (0-based).

      Returns:
          float: The area.
  )pydoc";
  iTrTin.def("triangle_area", &xms::TrTin::TriangleArea,triangle_area_doc,
        py::arg("tri"));
  // ---------------------------------------------------------------------------
  // function: next_boundary_point
  // ---------------------------------------------------------------------------
  const char* next_boundary_point_doc = R"pydoc(
      Returns the next point CW from point on the boundary. CCW if in an inside 
      hole. Compare to trNextBoundaryVertex (or trPreviousBoundaryVertex since 
      order here is CW, not CCW).

      Args:
        point (int): The starting point.

      Returns:
          int: The point index or XM_NONE if not found.
  )pydoc";
  iTrTin.def("next_boundary_point", &xms::TrTin::NextBoundaryPoint,
    next_boundary_point_doc,py::arg("point"));
  // ---------------------------------------------------------------------------
  // function: previous_boundary_point
  // ---------------------------------------------------------------------------
  const char* previous_boundary_point_doc = R"pydoc(
      Returns the next point CCW from point on the boundary. CW if in an inside 
      hole. Compare to trPreviousBoundaryVertex (or trNextBoundaryVertex since 
      order here is CW, not CCW).

      Args:
        point (int): The starting point.

      Returns:
          int: The point index or XM_NONE if not found.
  )pydoc";
  iTrTin.def("previous_boundary_point", &xms::TrTin::PreviousBoundaryPoint,
    previous_boundary_point_doc,py::arg("point"));
  // ---------------------------------------------------------------------------
  // function: get_boundary_points
  // ---------------------------------------------------------------------------
  const char* get_boundary_points_doc = R"pydoc(
      Gives the 0-based indices of all points on any boundary, in no particular 
      order.

      Returns:
          iterable: The 0-based indices of all points on the boundary.
  )pydoc";
  iTrTin.def("get_boundary_points", [](xms::TrTin &self) -> py::iterable {
          xms::VecInt bp;
          self.GetBoundaryPoints(bp);
          return xms::PyIterFromVecInt(bp);
      },get_boundary_points_doc);
  // ---------------------------------------------------------------------------
  // function: get_boundary_polys
  // ---------------------------------------------------------------------------
  const char* get_boundary_polys_doc = R"pydoc(
     Gets exterior boundary and all interior voids as polygons of 0-based point 
     indices. First point is not repeated as the last point.

      Returns:
          tuple: Outer polygon is CW, inner polys are CCW. First point is 
            repeated at the end.
  )pydoc";
  iTrTin.def("get_boundary_polys", [](xms::TrTin &self) -> py::tuple {
          xms::VecInt2d bp;
          self.GetBoundaryPolys(bp);
          return xms::PyIterFromVecInt2d(bp);
      },get_boundary_polys_doc);
  // ---------------------------------------------------------------------------
  // function: get_extents
  // ---------------------------------------------------------------------------
  const char* get_extents_doc = R"pydoc(
     Computes the extents (min, max) of the tin.

      Returns:
          tuple: Minimum xyz, maximum xyz
  )pydoc";
  iTrTin.def("get_extents", [](xms::TrTin &self) -> py::tuple {
          xms::Pt3d pt_min, pt_max;
          self.GetExtents(pt_min, pt_max);
          auto min_pt = xms::PyIterFromPt3d(pt_min);
          auto max_pt = xms::PyIterFromPt3d(pt_max);
          return py::make_tuple(min_pt, max_pt);
      },get_extents_doc);
  // ---------------------------------------------------------------------------
  // function: export_tin_file
  // ---------------------------------------------------------------------------
  const char* export_tin_file_doc = R"pydoc(
     Export in the .tin file format. Useful for debugging.

      Args:
          fname (str): The name of the file to export into
  )pydoc";
  iTrTin.def("export_tin_file", [](xms::TrTin &self, std::string fname) {
          // TODO: This needs some error checking
          std::filebuf fb;
          fb.open(fname, std::ios::out);
          std::ostream os(&fb);
          self.ExportTinFile(os);
          fb.close();
      },export_tin_file_doc,py::arg("fname"));
  // ---------------------------------------------------------------------------
  // function: swap_edge
  // ---------------------------------------------------------------------------
  const char* swap_edge_doc = R"pydoc(
     Swap edges if triangles combine to form convex quad. Compare to trSwapEdge.

      Args:
          tri_A (int): First triangle.
          tri_B (int): Second triangle.
          check_angle (bool): If true, won't swap if very thin triangle would be
            created.
  )pydoc";
  iTrTin.def("swap_edge", &xms::TrTin::SwapEdge,swap_edge_doc,
    py::arg("tri_A"), py::arg("tri_B"), py::arg("check_angle"));
  // ---------------------------------------------------------------------------
  // function: delete_triangles
  // ---------------------------------------------------------------------------
  const char* delete_triangles_doc = R"pydoc(
     Deletes the triangles specified in \a a_trisToDelete and updates and 
     renumbers triangle adjacency info.

      Args:
          tris (iterable): Triangles to be deleted.
  )pydoc";
  iTrTin.def("delete_triangles", [](xms::TrTin &self, py::iterable tris) {
          xms::SetInt to_delete;
          for (auto item : tris) {
            to_delete.insert(item.cast<int>());
          }
          self.DeleteTriangles(to_delete);
      },delete_triangles_doc,py::arg("tris"));
  // ---------------------------------------------------------------------------
  // function: delete_points
  // ---------------------------------------------------------------------------
  const char* delete_points_doc = R"pydoc(
     Deletes the points and any attached triangles, updates adjacency and 
     renumbers things.

      Args:
          pts (iterable): Points to be deleted.
  )pydoc";
  iTrTin.def("delete_points", [](xms::TrTin &self, py::iterable pts) {
          xms::SetInt to_delete;
          for (auto item : pts) {
            to_delete.insert(item.cast<int>());
          }
          self.DeletePoints(to_delete);
      },delete_points_doc, py::arg("pts"));
  // ---------------------------------------------------------------------------
  // function: optimize_triangulation
  // ---------------------------------------------------------------------------
  const char* optimize_triangulation_doc = R"pydoc(
     Swaps triangle edges until they are a Delauney triangulation.

      Returns:
          bool: True if modified.
  )pydoc";
  iTrTin.def("optimize_triangulation", &xms::TrTin::OptimizeTriangulation,
        optimize_triangulation_doc);
  // ---------------------------------------------------------------------------
  // function: build_tris_adj_to_pts
  // ---------------------------------------------------------------------------
  const char* build_tris_adj_to_pts_doc = R"pydoc(
     Build array of triangles adjacent to points.
  )pydoc";
  iTrTin.def("build_tris_adj_to_pts", &xms::TrTin::BuildTrisAdjToPts,
    build_tris_adj_to_pts_doc);
  // ---------------------------------------------------------------------------
  // function: clear
  // ---------------------------------------------------------------------------
  const char* clear_doc = R"pydoc(
     Delete the memory.
  )pydoc";
  iTrTin.def("clear", &xms::TrTin::Clear,clear_doc);
  // ---------------------------------------------------------------------------
  // function: from_string
  // ---------------------------------------------------------------------------
  const char* from_string_doc = R"pydoc(
     Use boost archive to turn the text into a TrTin.

     Args:
        text (str): The string.
  )pydoc";
  iTrTin.def("from_string", &xms::TrTin::FromString, from_string_doc,
    py::arg("text"));
  // ---------------------------------------------------------------------------
  // function: __str__
  // ---------------------------------------------------------------------------
  const char* __str___doc = R"pydoc(
     Use boost archive to get the TrTin as text.

     Returns:
        str: The string.
  )pydoc";
  iTrTin.def("__str__", &xms::TrTin::ToString,__str___doc);
}