
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
