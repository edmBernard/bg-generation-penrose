
#pragma once

#include <fmt/format.h>

#include <array>
#include <optional>
#include <string>

struct Point {
  float x;
  float y;
};

Point operator+(Point pt1, Point pt2) {
  return {pt1.x + pt2.x, pt1.y + pt2.y};
}
Point operator-(Point pt1, Point pt2) {
  return {pt1.x - pt2.x, pt1.y - pt2.y};
}
Point operator*(float value, Point pt) {
  return {value * pt.x, value * pt.y};
}
Point operator*(Point pt, float value) {
  return {value * pt.x, value * pt.y};
}
Point operator/(Point pt, float value) {
  return {pt.x / value, pt.y / value};
}
Point scalar(Point pt1, Point pt2) {
  return {pt1.x * pt2.x, pt1.y * pt2.y};
}

enum Color {
  // according to this documentation : https://tartarus.org/~simon/20110412-penrose/penrose.xhtml
  // we use these colors to identify triangles for the deflation algorithm
  kYellow,
  kRed,
  kBlue,
  kCyan
};

struct Triangle {
  std::array<Point, 3> vertices;

  Triangle(Point A, Point B, Point C) {
    vertices[0] = A;
    vertices[1] = B;
    vertices[2] = C;
  }
};

std::string to_string(const Point &pt) {
  return fmt::format("({}, {})", pt.x, pt.y);
}

std::string to_string(const Triangle &triangle) {
  return fmt::format("{}, {}, {}", to_string(triangle.vertices[0]), to_string(triangle.vertices[1]), to_string(triangle.vertices[2]));
}

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

} // namespace svg
