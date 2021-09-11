#pragma once

#include <penrose.hpp>
#include <vector>

[[nodiscard]] bool saveTiling(const std::vector<penrose::PenroseTriangle> &triangles, int canvasSize) {

  std::ofstream out("penrose_tiling.svg");
  if (!out) {
    std::cerr << "Cannot open output file.\n";
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
