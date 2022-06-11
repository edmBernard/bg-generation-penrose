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
  bool flag;

  PenroseTriangle(TriangleKind color, Point A, Point B, Point C, bool flag = false)
      : Triangle(A, B, C), color(color), flag(flag) {
  }

};

struct PenroseQuadrilateral : Quadrilateral {
  TriangleKind color;
  bool flag;

  PenroseQuadrilateral(TriangleKind color, Point A, Point B, Point C, Point D, bool flag)
      : Quadrilateral(A, B, C, D), color(color), flag(flag) {
  }
};

std::string to_string(const PenroseTriangle &triangle) {
  // return fmt::format("{}, {}, {}, {}", triangle.color, to_string(triangle.vertices[0]), to_string(triangle.vertices[1]), to_string(triangle.vertices[2]));
  return "";
}

std::vector<PenroseTriangle> deflate(const PenroseTriangle &triangle) {
  const Point A = triangle.vertices[0];
  const Point B = triangle.vertices[1];
  const Point C = triangle.vertices[2];

  switch (triangle.color) {
  case TriangleKind::kDart: {
    const Point R = A + (B - A) / goldenRatio;
    const Point Q = B + (C - B) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {TriangleKind::kDart, R, A, Q, triangle.flag},
        {TriangleKind::kDart, C, A, Q, triangle.flag},
        {TriangleKind::kKite, Q, B, R, triangle.flag}};
  } break;
  case TriangleKind::kKite: {
    const Point P = B + (A - B) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {TriangleKind::kKite, C, A, P, triangle.flag},
        {TriangleKind::kDart, P, B, C, triangle.flag}};
  } break;
  case TriangleKind::kRhombsCyan: {
    const Point P = A + (B - A) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {TriangleKind::kRhombsCyan, C, P, B, triangle.flag},
        {TriangleKind::kRhombsViolet, P, C, A, triangle.flag}};
  } break;
  case TriangleKind::kRhombsViolet: {
    const Point Q = B + (A - B) / goldenRatio;
    const Point R = B + (C - B) / goldenRatio;
    return std::vector<PenroseTriangle>{
        {TriangleKind::kRhombsViolet, R, C, A, triangle.flag},
        {TriangleKind::kRhombsViolet, Q, R, B, triangle.flag},
        {TriangleKind::kRhombsCyan, R, Q, A, triangle.flag}};
  } break;
  default:
    throw std::runtime_error("Unknown penrose type");
    break;
  }
}

std::vector<PenroseTriangle> deflate(const std::vector<PenroseTriangle> &triangles) {
  std::vector<PenroseTriangle> newList;
  for (const auto &triangle : triangles) {
    const auto small = deflate(triangle);
    std::move(small.begin(), small.end(), std::back_inserter(newList));
  }
  return newList;
}

PenroseQuadrilateral completeShape(const PenroseTriangle &triangle) {
    const Point A = triangle.vertices[0];
    const Point B = triangle.vertices[1];
    const Point C = triangle.vertices[2];
    const Point D = A + 2 * ((B-A) + (C-B) * scalar(A-B, C-B) / scalar(C-B, C-B));
    return {triangle.color, A, B, C, D, triangle.flag};
}

std::vector<PenroseQuadrilateral> completeShape(const std::vector<PenroseTriangle> &triangles) {
  std::vector<PenroseQuadrilateral> newList;
  for (const auto &triangle : triangles) {
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
      {triangle.color, A, B, C, triangle.flag},
      {triangle.color, D, B, C, triangle.flag}
    };
}

std::vector<PenroseTriangle> splitShape(const std::vector<PenroseQuadrilateral> &quadrilaterals) {
  std::vector<PenroseTriangle> newList;
  for (const auto &quad : quadrilaterals) {
    const auto triangles = splitShape(quad);
    std::move(triangles.begin(), triangles.end(), std::back_inserter(newList));
  }
  return newList;
}

Point moveMargin(const Point& A, const Point& B, const Point& C, const Point& D, const float margin) {
  // D is here only to indicate margin direction
  const Point AO = (C-A) / norm(C-A) + (B-A) / norm(B-A);
  const float direction = scalar(AO, D-A) > 0.f ? 1.f : -1.f;
  const Point u = turn90(C-A);
  const float pr = scalar(AO / norm(AO), u / norm(u));
  const Point Am = A + AO / norm(AO) * margin / std::abs(pr) * direction;
  const float finalMarge = scalar(Am-A, u / norm(u));
  return Am;
}

std::vector<PenroseQuadrilateral> addMargin(const std::vector<PenroseQuadrilateral> &quadrilaterals, const float margin) {
  std::vector<PenroseQuadrilateral> newList;
  for (const auto& quad : quadrilaterals) {
    const Point A = quad.vertices[0];
    const Point B = quad.vertices[1];
    const Point C = quad.vertices[2];
    const Point D = quad.vertices[3];
    newList.emplace_back(quad.color, moveMargin(A, B, C, D, margin), moveMargin(B, D, A, C, margin), moveMargin(C, A, D, B, margin), moveMargin(D, C, B, A, margin), quad.flag);
  }
  return newList;
}

std::vector<PenroseQuadrilateral> deflateAndMerge(const std::vector<PenroseTriangle> &triangles, int level) {
  std::vector<PenroseTriangle> tiling = deflate(triangles);
  for (int l = 1; l < level; ++l) {
    tiling = deflate(tiling);
  }

  std::vector<PenroseQuadrilateral> quadTiling = completeShape(tiling);

  // remove duplicate
  std::sort(quadTiling.begin(), quadTiling.end());
  auto last = std::unique(quadTiling.begin(), quadTiling.end());
  quadTiling.erase(last, quadTiling.end());

  return quadTiling;
}

void setRandomFlag(std::vector<PenroseQuadrilateral> &quadrilaterals, int threshold = 5) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 10);
  for (auto &quad : quadrilaterals) {
    quad.flag = distrib(gen) >= threshold ? quad.flag : !quad.flag;
  }
}



} // namespace penrose