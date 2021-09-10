#pragma once

#include <geometry.hpp>

#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iterator>

namespace penrose {

constexpr float pi = 3.14159265358979f;
constexpr float goldenRatio = 1.618033988749895f;


struct PenroseTriangle : Triangle {
  Color color;

  PenroseTriangle(Color color, Point A, Point B, Point C)
  : Triangle(A, B, C)
  , color(color) {}
};


std::string to_string(const PenroseTriangle& triangle) {
  return fmt::format("{}, {}, {}, {}", triangle.color, to_string(triangle.vertices[0]), to_string(triangle.vertices[1]), to_string(triangle.vertices[2]));
}

std::vector<PenroseTriangle> deflate(const PenroseTriangle &triangle) {
  switch (triangle.color) {
  case Color::kBlue:
  case Color::kYellow:
  case Color::kRed:
  case Color::kCyan:
    return {};
    break;
  default:
    throw std::runtime_error("Unknown penrose color");
    break;
  }
}

std::vector<PenroseTriangle> deflate(const std::vector<PenroseTriangle> &triangles) {
  std::vector<PenroseTriangle> newList;
  for (const auto triangle : triangles) {
    const auto small = deflate(triangle);
    std::move(small.begin(), small.end(), std::back_inserter(newList));
  }
  return newList;
}

} // namespace penrose