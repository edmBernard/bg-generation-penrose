
#pragma once

#include <fmt/format.h>

#include <array>
#include <optional>
#include <string>

struct Point {
  float x;
  float y;
};

bool operator==(const Point& lhs, const Point& rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}
bool operator<(const Point& lhs, const Point& rhs) {
  if (lhs.x == rhs.x)
    return lhs.y < rhs.y;
  return lhs.x < rhs.x;
}

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
float scalar(Point pt1, Point pt2) {
  return pt1.x * pt2.x + pt1.y * pt2.y;
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

bool operator==(const Triangle& lhs, const Triangle& rhs) {
  return rhs.vertices[0] == lhs.vertices[0] &&
         rhs.vertices[1] == lhs.vertices[1] &&
         rhs.vertices[2] == lhs.vertices[2];
}

struct Quadrilateral {
  std::array<Point, 4> vertices;

  Quadrilateral(Point A, Point B, Point C, Point D) {
    vertices[0] = A;
    vertices[1] = B;
    vertices[2] = C;
    vertices[3] = D;
  }
};

bool operator==(const Quadrilateral& lhs, const Quadrilateral& rhs) {
  return rhs.vertices[0] == lhs.vertices[0] &&
         rhs.vertices[1] == lhs.vertices[1] &&
         rhs.vertices[2] == lhs.vertices[2] &&
         rhs.vertices[3] == lhs.vertices[3];
}

bool operator<(const Quadrilateral& lhs, const Quadrilateral& rhs) {
  if (rhs.vertices[0] == lhs.vertices[0])
    if (rhs.vertices[1] == lhs.vertices[1])
      if (rhs.vertices[2] == lhs.vertices[2])
        return rhs.vertices[3] == lhs.vertices[3];
      return rhs.vertices[2] < lhs.vertices[2];
    return rhs.vertices[1] < lhs.vertices[1];
  return rhs.vertices[0] < lhs.vertices[0];
}

std::string to_string(const Point &pt) {
  return fmt::format("({}, {})", pt.x, pt.y);
}

std::string to_string(const Triangle &triangle) {
  return fmt::format("{}, {}, {}", to_string(triangle.vertices[0]), to_string(triangle.vertices[1]), to_string(triangle.vertices[2]));
}

std::string to_string(const Quadrilateral &quad) {
  return fmt::format("{}, {}, {}, {}", to_string(quad.vertices[0]), to_string(quad.vertices[1]), to_string(quad.vertices[2]), to_string(quad.vertices[3]));
}
