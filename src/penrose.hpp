#pragma once

#include <geometry.hpp>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace penrose {

constexpr float pi = 3.14159265358979f;
constexpr float goldenRatio = 1.618033988749895f;

struct PenroseTriangle : Triangle {
  Color color;

  PenroseTriangle(Color color, Point A, Point B, Point C)
      : Triangle(A, B, C), color(color) {
  }
};

struct PenroseQuadrilateral : Quadrilateral {
  Color color;

  PenroseQuadrilateral(Color color, Point A, Point B, Point C, Point D)
      : Quadrilateral(A, B, C, D), color(color) {
  }
};

std::string to_string(const PenroseTriangle &triangle) {
  return fmt::format("{}, {}, {}, {}", triangle.color, to_string(triangle.vertices[0]), to_string(triangle.vertices[1]), to_string(triangle.vertices[2]));
}

std::vector<PenroseTriangle> deflate(const PenroseTriangle &triangle) {
  const Point A = triangle.vertices[0];
  const Point B = triangle.vertices[1];
  const Point C = triangle.vertices[2];

  switch (triangle.color) {
  case Color::kRed: {
    const Point R = A + (B - A) / goldenRatio;
    const Point Q = B + (C - B) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {kRed, R, A, Q},
        {kRed, C, A, Q},
        {kYellow, Q, B, R}};
  } break;
  case Color::kYellow: {
    const Point P = B + (A - B) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {kYellow, C, A, P},
        {kRed, P, B, C}};
  } break;
  case Color::kBlue:
  case Color::kCyan:
  default:
    throw std::runtime_error("Unknown penrose color");
    break;
  }
}


PenroseQuadrilateral completeShape(const PenroseTriangle &triangle) {
    const Point A = triangle.vertices[0];
    const Point B = triangle.vertices[1];
    const Point C = triangle.vertices[2];
    const Point D = A + 2 * ((B-A) + (C-B) * scalar(A-B, C-B) / scalar(C-B, C-B));
    return {triangle.color, A, B, C, D};
}

std::vector<PenroseTriangle> deflate(const std::vector<PenroseTriangle> &triangles) {
  std::vector<PenroseTriangle> newList;
  for (const auto triangle : triangles) {
    const auto small = deflate(triangle);
    std::move(small.begin(), small.end(), std::back_inserter(newList));
  }
  return newList;
}

std::vector<PenroseQuadrilateral> completeShape(const std::vector<PenroseTriangle> &triangles) {
  std::vector<PenroseQuadrilateral> newList;
  for (const auto triangle : triangles) {
    newList.push_back(completeShape(triangle));
  }
  return newList;
}

} // namespace penrose