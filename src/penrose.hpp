//
//  https://github.com/edmBernard/bg-generation-penrose
//
//  Created by Erwan BERNARD on 11/09/2021.
//
//  Copyright (c) 2021 Erwan BERNARD. All rights reserved.
//  Distributed under the Apache License, Version 2.0. (See accompanying
//  file LICENSE or copy at http://www.apache.org/licenses/LICENSE-2.0)
//

#pragma once

#include <geometry.hpp>

#include <algorithm>
#include <iterator>
#include <stdexcept>
#include <vector>
#include <random>

namespace penrose {

constexpr float pi = 3.14159265358979f;
constexpr float goldenRatio = 1.618033988749895f;
constexpr int levelToFix = 6;

enum class TriangleKind {
  // according to this documentation : https://tartarus.org/~simon/20110412-penrose/penrose.xhtml
  // we use these colors to identify triangles for the deflation algorithm
  kKite,
  kDart,
  kRhombsCyan,
  kRhombsViolet
};

inline bool isSmall(TriangleKind kind) {
  return kind == TriangleKind::kKite || kind == TriangleKind::kRhombsCyan;
}

struct PenroseTriangle : Triangle {
  TriangleKind color;
  TriangleKind parent;

  PenroseTriangle(TriangleKind color, Point A, Point B, Point C)
      : Triangle(A, B, C), color(color) {
  }

  PenroseTriangle(TriangleKind color, Point A, Point B, Point C, TriangleKind parent)
      : Triangle(A, B, C), color(color), parent(parent) {
  }
};

struct PenroseQuadrilateral : Quadrilateral {
  TriangleKind color;
  TriangleKind parent;

  PenroseQuadrilateral(TriangleKind color, Point A, Point B, Point C, Point D, TriangleKind parent)
      : Quadrilateral(A, B, C, D), color(color), parent(parent) {
  }
};

std::string to_string(const PenroseTriangle &triangle) {
  return fmt::format("{}, {}, {}, {}", triangle.color, to_string(triangle.vertices[0]), to_string(triangle.vertices[1]), to_string(triangle.vertices[2]));
}

std::vector<PenroseTriangle> deflate(const PenroseTriangle &triangle, int level) {
  const Point A = triangle.vertices[0];
  const Point B = triangle.vertices[1];
  const Point C = triangle.vertices[2];
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<> distrib(0, 10);
  const TriangleKind parentKind = (levelToFix == level) ? ((distrib(gen) < 5) ? TriangleKind::kDart : TriangleKind::kKite) : triangle.parent;

  switch (triangle.color) {
  case TriangleKind::kDart: {
    const Point R = A + (B - A) / goldenRatio;
    const Point Q = B + (C - B) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {TriangleKind::kDart, R, A, Q, parentKind},
        {TriangleKind::kDart, C, A, Q, parentKind},
        {TriangleKind::kKite, Q, B, R, parentKind}};
  } break;
  case TriangleKind::kKite: {
    const Point P = B + (A - B) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {TriangleKind::kKite, C, A, P, parentKind},
        {TriangleKind::kDart, P, B, C, parentKind}};
  } break;
  case TriangleKind::kRhombsCyan: {
    const Point P = A + (B - A) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {TriangleKind::kRhombsCyan, C, P, B, parentKind},
        {TriangleKind::kRhombsViolet, P, C, A, parentKind}};
  } break;
  case TriangleKind::kRhombsViolet: {
    const Point Q = B + (A - B) / goldenRatio;
    const Point R = B + (C - B) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {TriangleKind::kRhombsViolet, R, C, A, parentKind},
        {TriangleKind::kRhombsViolet, Q, R, B, parentKind},
        {TriangleKind::kRhombsCyan, R, Q, A, parentKind}};
  } break;
  default:
    throw std::runtime_error("Unknown penrose type");
    break;
  }
}



std::vector<PenroseTriangle> deflate(const std::vector<PenroseTriangle> &triangles, int level) {
  std::vector<PenroseTriangle> newList;
  for (const auto triangle : triangles) {
    const auto small = deflate(triangle, level);
    std::move(small.begin(), small.end(), std::back_inserter(newList));
  }
  return newList;
}

PenroseQuadrilateral completeShape(const PenroseTriangle &triangle) {
    const Point A = triangle.vertices[0];
    const Point B = triangle.vertices[1];
    const Point C = triangle.vertices[2];
    const Point D = A + 2 * ((B-A) + (C-B) * scalar(A-B, C-B) / scalar(C-B, C-B));
    return {triangle.color, A, B, C, D, triangle.parent};
}

std::vector<PenroseQuadrilateral> completeShape(const std::vector<PenroseTriangle> &triangles) {
  std::vector<PenroseQuadrilateral> newList;
  for (const auto triangle : triangles) {
    newList.push_back(completeShape(triangle));
  }
  return newList;
}

std::vector<PenroseTriangle> splitShape(const PenroseQuadrilateral &triangle) {
    const Point A = triangle.vertices[0];
    const Point B = triangle.vertices[1];
    const Point C = triangle.vertices[2];
    const Point D = triangle.vertices[3];
    return {
      {triangle.color, A, B, C, triangle.parent},
      {triangle.color, D, B, C, triangle.parent}
    };
}

std::vector<PenroseTriangle> splitShape(const std::vector<PenroseQuadrilateral> &quadrilaterals) {
  std::vector<PenroseTriangle> newList;
  for (const auto quad : quadrilaterals) {
    const auto triangles = splitShape(quad);
    newList.push_back(triangles[0]);
    newList.push_back(triangles[1]);
  }
  return newList;
}

std::vector<PenroseQuadrilateral> deflateAndMerge(const std::vector<PenroseTriangle> &triangles, int level) {
  std::vector<PenroseTriangle> tiling = deflate(triangles, 0);

  for (int l = 0; l < level; ++l) {
    tiling = deflate(tiling, l);
  }

  std::vector<PenroseQuadrilateral> quadTiling = completeShape(tiling);

  // remove duplicate
  std::sort(quadTiling.begin(), quadTiling.end());
  auto last = std::unique(quadTiling.begin(), quadTiling.end());
  quadTiling.erase(last, quadTiling.end());

  return quadTiling;
}

} // namespace penrose