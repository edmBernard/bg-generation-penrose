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

struct RGB {
  int r;
  int g;
  int b;
};

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

[[nodiscard]] bool saveTiling(const std::vector<penrose::PenroseTriangle> &triangles, int canvasSize, RGB rgb1, RGB rgb2, RGB background) {

  std::ofstream out("penrose_tiling.svg");
  if (!out) {
    spdlog::error("Cannot open output file.");
    return false;
  }

  const float strokesWidth = norm(triangles[0].vertices[0]-triangles[0].vertices[1]) / 100.0f;

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << fmt::format("height='{}' width='{}'>\n", canvasSize, canvasSize)
      << fmt::format("<rect height='100%' width='100%' fill='rgb({},{},{})'/>\n", background.r, background.g, background.b)
      << "<g id='surface1'>\n";
  out << to_path(triangles, Fill{rgb1.r, rgb1.g, rgb1.b}, Strockes{0, 0, 0, strokesWidth}, [](const penrose::PenroseTriangle &tr) { return tr.color == penrose::TriangleKind::kKite; });
  out << to_path(triangles, Fill{rgb2.r, rgb2.g, rgb2.b}, Strockes{0, 0, 0, strokesWidth}, [](const penrose::PenroseTriangle &tr) { return tr.color == penrose::TriangleKind::kDart; });
  out << "</g>\n</svg>\n";
  return true;
}

[[nodiscard]] bool saveTiling(const std::vector<penrose::PenroseQuadrilateral> &quad, int canvasSize, RGB rgb1, RGB rgb2, RGB background, int threshold = 6) {

  std::ofstream out("penrose_tiling_q.svg");
  if (!out) {
    spdlog::error("Cannot open output file.");
    return false;
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 10);

  const float strokesWidth = norm(quad[0].vertices[0]-quad[0].vertices[1]) / 100.0f;

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << fmt::format("height='{}' width='{}'>\n", canvasSize, canvasSize)
      << fmt::format("<rect height='100%' width='100%' fill='rgb({},{},{})'/>\n", background.r, background.g, background.b)
      << "<g id='surface1'>\n";
  out << to_path(quad, Fill{rgb1.r, rgb1.g, rgb1.b}, {}, [&](const penrose::PenroseQuadrilateral &tr) { return tr.color == penrose::TriangleKind::kKite ? distrib(gen) > threshold : false; });
  out << to_path(quad, Fill{rgb2.r, rgb2.g, rgb2.b}, {}, [&](const penrose::PenroseQuadrilateral &tr) { return tr.color == penrose::TriangleKind::kDart ? distrib(gen) > threshold : false; });
  out << to_path(quad, {}, Strockes{0, 0, 0, strokesWidth});
  out << "</g>\n</svg>\n";
  return true;
}

} // namespace svg