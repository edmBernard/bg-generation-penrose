#pragma once

#include <penrose.hpp>

#include <spdlog/spdlog.h>

#include <fstream>
#include <vector>

namespace svg {
namespace details {

std::string to_path(const Triangle &tr) {
  // we don't close the path at the end, this allow to draw border on only 2 sides of the triangle
  return fmt::format("M {} {} L {} {} L {} {}", tr.vertices[2].x, tr.vertices[2].y, tr.vertices[0].x, tr.vertices[0].y, tr.vertices[1].x, tr.vertices[1].y);
}
} // namespace details

struct Fill {
  int r;
  int g;
  int b;
};

struct Strockes {
  int r;
  int g;
  int b;
  float width;
};

template <typename T, typename Lambda>
std::string to_path(const std::vector<T> &triangles, std::optional<Fill> fill, std::optional<Strockes> strockes, Lambda func) {
  std::string s_path;
  for (auto &tr : triangles) {
    if (func(tr)) {
      s_path += details::to_path(tr) + " ";
    }
  }
  std::string s_fill = fill ? fmt::format("fill:rgb({},{},{})", fill->r, fill->g, fill->b) : "";
  std::string s_strockes = strockes ? fmt::format("stroke:rgb({},{},{});stroke-width:{}", strockes->r, strockes->g, strockes->b, strockes->width) : "";
  return fmt::format("<path style='{};{}' d='{}'></path>\n", s_fill, s_strockes, s_path);
}

[[nodiscard]] bool saveTiling(const std::vector<penrose::PenroseTriangle> &triangles, int canvasSize) {

  std::ofstream out("penrose_tiling_v3.svg");
  if (!out) {
    spdlog::error("Cannot open output file.");
    return false;
  }

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << "height='" << canvasSize << "' width='" << canvasSize << "'>\n"
      << "<g id='surface1'>\n";
  out << to_path(triangles, Fill{255, 80, 80}, Strockes{0, 0, 0, 1}, [](const penrose::PenroseTriangle &tr) { return tr.color == Color::kRed; });
  out << to_path(triangles, Fill{255, 255, 80}, Strockes{0, 0, 0, 1}, [](const penrose::PenroseTriangle &tr) { return tr.color == Color::kYellow; });
  out << "</g>\n</svg>\n";
  return true;
}

} // namespace svg