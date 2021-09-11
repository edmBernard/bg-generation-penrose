#pragma once

#include <penrose.hpp>

#include <spdlog/spdlog.h>

#include <fstream>
#include <vector>

[[nodiscard]] bool saveTiling(const std::vector<penrose::PenroseTriangle> &triangles, int canvasSize) {

  std::ofstream out("penrose_tiling_v3.svg");
  if (!out) {
    spdlog::error("Cannot open output file.");
    return false;
  }

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << "height='" << canvasSize << "' width='" << canvasSize << "'>\n"
      << "<g id='surface1'>\n";
  out << svg::to_path(triangles, svg::Fill{255, 80, 80}, svg::Strockes{0, 0, 0, 1}, [](const penrose::PenroseTriangle &tr) { return tr.color == Color::kRed; });
  out << svg::to_path(triangles, svg::Fill{255, 255, 80}, svg::Strockes{0, 0, 0, 1}, [](const penrose::PenroseTriangle &tr) { return tr.color == Color::kYellow; });
  out << "</g>\n</svg>\n";
  return true;
}
