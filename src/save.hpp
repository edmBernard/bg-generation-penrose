#pragma once

#include <penrose.hpp>

#include <spdlog/spdlog.h>

#include <fstream>
#include <vector>
#include <random>

namespace svg {
namespace details {

std::string to_path(const Triangle &tr) {
  // we don't close the path at the end, this allow to draw border on only 2 sides of the triangle
  // we don't want to draw the border between 2nd and 3rd vertices
  return fmt::format("M {} {} L {} {} L {} {}", tr.vertices[2].x, tr.vertices[2].y, tr.vertices[0].x, tr.vertices[0].y, tr.vertices[1].x, tr.vertices[1].y);
}
std::string to_path(const Quadrilateral &tr) {
  return fmt::format("M {} {} L {} {} L {} {} L {} {} Z", tr.vertices[0].x, tr.vertices[0].y, tr.vertices[1].x, tr.vertices[1].y, tr.vertices[3].x, tr.vertices[3].y, tr.vertices[2].x, tr.vertices[2].y);
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

template <typename T, typename Lambda = std::function<bool(T)>>
std::string to_path(const std::vector<T> &shape, std::optional<Fill> fill, std::optional<Strockes> strockes, Lambda func = [](const T &){ return true; }) {
  std::string s_path;
  for (auto &tr : shape) {
    if (func(tr)) {
      s_path += details::to_path(tr) + " ";
    }
  }
  std::string s_fill = fill ? fmt::format("fill:rgb({},{},{})", fill->r, fill->g, fill->b) : "fill:none";
  std::string s_strockes = strockes ? fmt::format("stroke:rgb({},{},{});stroke-width:{}", strockes->r, strockes->g, strockes->b, strockes->width) : "";
  return fmt::format("<path style='{};{}' d='{}'></path>\n", s_fill, s_strockes, s_path);
}

[[nodiscard]] bool saveTiling(const std::vector<penrose::PenroseTriangle> &triangles, int canvasSize) {

  std::ofstream out("penrose_tiling.svg");
  if (!out) {
    spdlog::error("Cannot open output file.");
    return false;
  }

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << "height='" << canvasSize << "' width='" << canvasSize << "'>\n"
      << "<g id='surface1'>\n";
  out << to_path(triangles, Fill{255, 80, 80}, Strockes{0, 0, 0, 1}, [](const penrose::PenroseTriangle &tr) { return tr.color == penrose::TriangleKind::kDart; });
  out << to_path(triangles, Fill{255, 255, 80}, Strockes{0, 0, 0, 1}, [](const penrose::PenroseTriangle &tr) { return tr.color == penrose::TriangleKind::kKite; });
  out << "</g>\n</svg>\n";
  return true;
}

[[nodiscard]] bool saveTiling(const std::vector<penrose::PenroseQuadrilateral> &quad, int canvasSize, int threshold = 3) {

  std::ofstream out("penrose_tiling_q.svg");
  if (!out) {
    spdlog::error("Cannot open output file.");
    return false;
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 10);

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << "height='" << canvasSize << "' width='" << canvasSize << "'>\n"
      << "<g id='surface1'>\n";
  out << to_path(quad, Fill{255, 80, 80}, {}, [&](const penrose::PenroseQuadrilateral &tr) { return tr.color == penrose::TriangleKind::kDart ? distrib(gen) > threshold : false; });
  out << to_path(quad, Fill{255, 255, 80}, {}, [&](const penrose::PenroseQuadrilateral &tr) { return tr.color == penrose::TriangleKind::kKite ? distrib(gen) > threshold : false; });
  out << to_path(quad, {}, Strockes{0, 0, 0, norm(quad[0].vertices[0]-quad[0].vertices[1]) / 100.0f});
  out << "</g>\n</svg>\n";
  return true;
}

} // namespace svg