
#pragma once

#include <fmt/format.h>

#include <array>
#include <optional>
#include <string>

constexpr float epsilon = 0.1f; // use for the comparison between Point

struct Point {
  float x;
  float y;
};


Point operator+(const Point &pt1, const Point & pt2) {
  return {pt1.x + pt2.x, pt1.y + pt2.y};
}
Point operator-(const Point & pt1, const Point & pt2) {
  return {pt1.x - pt2.x, pt1.y - pt2.y};
}
Point operator*(float value, const Point & pt) {
  return {value * pt.x, value * pt.y};
}
Point operator*(const Point & pt, float value) {
  return {value * pt.x, value * pt.y};
}
Point operator/(const Point & pt, float value) {
  return {pt.x / value, pt.y / value};
}
float scalar(const Point & pt1, const Point & pt2) {
  return pt1.x * pt2.x + pt1.y * pt2.y;
}
float norm(const Point & pt1) {
  return pt1.x * pt1.x + pt1.y * pt1.y;
}
bool operator==(const Point &lhs, const Point &rhs) {
  return norm(lhs - rhs) < epsilon;
}
bool operator<(const Point &lhs, const Point &rhs) {
  if (abs(lhs.x - rhs.x) < epsilon)
    return lhs.y < rhs.y;
  return lhs.x < rhs.x;
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

  Point center() {
    return (this->vertices[0] + this->vertices[1] + this->vertices[2]) / 3.;
  }
};

bool operator==(const Triangle &lhs, const Triangle &rhs) {
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

  Point center() const {
    return (this->vertices[0] + this->vertices[1] + this->vertices[2] + this->vertices[3]) / 4.;
  }
};

bool operator==(const Quadrilateral &lhs, const Quadrilateral &rhs) {
  // we compare gravity center approximative be enough
  return lhs.center() == rhs.center();
}

bool operator<(const Quadrilateral &lhs, const Quadrilateral &rhs) {
  // we compare gravity center approximative be enough
  return lhs.center() < rhs.center();
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
