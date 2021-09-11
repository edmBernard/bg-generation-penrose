#pragma once

#include <penrose.hpp>

#include <spdlog/spdlog.h>

#include <fstream>
#include <vector>

[[nodiscard]] bool saveTiling(const std::vector<penrose::PenroseTriangle> &triangles, int canvasSize) {

  std::ofstream out("penrose_tiling.svg");
  if (!out) {
    spdlog::error("Cannot open output file.");
    return false;
  }

  out << "<svg xmlns='http://www.w3.org/2000/svg' height='" << canvasSize
      << "' width='" << canvasSize << "'>\n"
      << "<rect height='100%' width='100%' fill='black'/>\n"
      << "<g stroke='rgb(255,165,0)'>\n";
  for (const auto &tr : triangles) {
    out << fmt::format("<polygon points='{},{} {},{} {},{}' fill='rgb(255,80,80)' stroke-width='1' stroke='rgb(255,80,80)'></polygon>\n",
           tr.vertices[2].x, tr.vertices[2].y, tr.vertices[0].x, tr.vertices[0].y, tr.vertices[1].x, tr.vertices[1].y);
  }
  for (const auto &tr : triangles) {
    out << fmt::format("<line x1='{:.3f}' y1='{:.3f}' x2='{:.3f}' y2='{:.3f}'/>\n", tr.vertices[0].x, tr.vertices[0].y, tr.vertices[1].x, tr.vertices[1].y);
    // out << fmt::format("<line x1='{:.3f}' y1='{:.3f}' x2='{:.3f}' y2='{:.3f}'/>\n", tr.vertices[1].x, tr.vertices[1].y, tr.vertices[2].x, tr.vertices[2].y);
    out << fmt::format("<line x1='{:.3f}' y1='{:.3f}' x2='{:.3f}' y2='{:.3f}'/>\n", tr.vertices[2].x, tr.vertices[2].y, tr.vertices[0].x, tr.vertices[0].y);
  }
  out << "</g>\n</svg>\n";
  return true;
}

[[nodiscard]] bool saveTiling_v2(const std::vector<penrose::PenroseTriangle> &triangles, int canvasSize) {

  std::ofstream out("penrose_tiling_v2.svg");
  if (!out) {
    spdlog::error("Cannot open output file.");
    return false;
  }

  out << "<svg xmlns='http://www.w3.org/2000/svg' height='" << canvasSize
      << "' width='" << canvasSize << "'>\n"
      << "<rect height='100%' width='100%' fill='black'/>\n"
      << "<g id='surface1'>";
  out << "<path style='stroke:none;fill-rule:nonzero;fill:rgb(54.901961%,54.901961%,54.901961%);fill-opacity:1;'"
      << " d='";
  for (const auto &tr : triangles) {
    out << fmt::format("M {} {} L {} {} L {} {} Z ", tr.vertices[2].x, tr.vertices[2].y, tr.vertices[0].x, tr.vertices[0].y, tr.vertices[1].x, tr.vertices[1].y);
  }
  out << "'> </path>\n";


  out << "<path style='fill:none;stroke-width:0.0412023;stroke-linecap:butt;stroke-linejoin:round;stroke:rgb(0%,0%,0%);stroke-opacity:1;stroke-miterlimit:10;'"
      << " d='";
  for (const auto &tr : triangles) {
    out << fmt::format("M {} {} L {} {} L {} {} Z ", tr.vertices[2].x, tr.vertices[2].y, tr.vertices[0].x, tr.vertices[0].y, tr.vertices[1].x, tr.vertices[1].y);
  }
  out << "'> </path>\n";
  out << "</g>\n</svg>\n";
  return true;
}

[[nodiscard]] bool saveTiling_v3(const std::vector<penrose::PenroseTriangle> &triangles, int canvasSize) {

  std::ofstream out("penrose_tiling_v3.svg");
  if (!out) {
    spdlog::error("Cannot open output file.");
    return false;
  }

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << "height='" << canvasSize << "' width='" << canvasSize << "'>\n"
      << "<g id='surface1'>\n";
  out << svg::to_path(triangles, svg::Fill{255, 80, 80}, svg::Strockes{0, 0, 0, 1}, [](const penrose::PenroseTriangle& tr){ return tr.color == Color::kRed; });
  out << svg::to_path(triangles, svg::Fill{255, 255, 80}, svg::Strockes{0, 0, 0, 1}, [](const penrose::PenroseTriangle& tr){ return tr.color == Color::kYellow; });
  out << "</g>\n</svg>\n";
  return true;
}
