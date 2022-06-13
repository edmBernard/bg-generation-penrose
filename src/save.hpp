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

#include <penrose.hpp>

#include <spdlog/spdlog.h>
#include <fmt/ostream.h>

#include <fstream>
#include <random>
#include <vector>
#include <type_traits>

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

  friend std::ostream &operator<<(std::ostream &os, const RGB &rgb ) {
    return os << fmt::format("rgb({},{},{})", rgb.r, rgb.b, rgb.b);
  }
};

struct Fill {
  RGB color;

  Fill(int r, int g, int b)
      : color{r, g, b} {
  }

  Fill(RGB rgb)
      : color{rgb} {
  }

  friend std::ostream &operator<<(std::ostream &os, const Fill &fill ) {
    return os << fmt::format("fill:", fill.color);
  }
};

struct StrokesStyle {
  RGB color;
  float width;

  StrokesStyle(int r, int g, int b, float width)
      : color{r, g, b}, width(width) {
  }

  StrokesStyle(RGB rgb, float width)
      : color{rgb}, width(width) {
  }

  friend std::ostream &operator<<(std::ostream &os, const StrokesStyle &rgb ) {
    return os << fmt::format("stroke:{};stroke-width:{};stroke-linecap:butt;stroke-linejoin:miter", rgb.color, rgb.width);
  }
};

template <typename T, typename Lambda = std::function<bool(T, size_t)>>
std::string to_path(
    const std::vector<T> &shape, std::optional<Fill> fill, std::optional<StrokesStyle> strockes, Lambda func = [](const T &, size_t) { return true; }) {
  std::string s_path;
  for (size_t idx = 0; idx < shape.size(); ++idx) {
    if (func(shape[idx], idx)) {
      s_path += details::to_path(shape[idx]) + " ";
    }
  }
  std::string s_fill = fill ? fmt::format("fill:rgb({},{},{})", fill->color.r, fill->color.g, fill->color.b) : "fill:none";
  std::string s_strockes = strockes ? fmt::format("stroke:rgb({},{},{});stroke-width:{};stroke-linecap:butt;stroke-linejoin:miter", strockes->color.r, strockes->color.g, strockes->color.b, strockes->width) : "";
  return fmt::format("<path style='{};{}' d='{}'></path>\n", s_fill, s_strockes, s_path);
}

[[nodiscard]] bool saveTiling(const std::string &filename, const std::vector<penrose::PenroseTriangle> &triangles, int canvasSize,
                              RGB rgb1, RGB rgb2, RGB background) {

  std::ofstream out(filename);
  if (!out) {
    spdlog::error("Cannot open output file : {}", filename);
    return false;
  }

  const float strokesWidth = std::sqrt(normSq(triangles[0].vertices[0] - triangles[0].vertices[1])) / 15.0f;

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << fmt::format("height='{size}' width='{size}' viewBox='0 0 {size} {size}'>\n", fmt::arg("size", canvasSize))
      << fmt::format("<rect height='100%' width='100%' fill='rgb({},{},{})'/>\n", background.r, background.g, background.b)
      << "<g id='surface1'>\n";
  out << to_path(triangles, Fill{rgb1}, StrokesStyle{0, 0, 0, strokesWidth}, [](const penrose::PenroseTriangle &tr, size_t) { return isSmall(tr.color); });
  out << to_path(triangles, Fill{rgb2}, StrokesStyle{0, 0, 0, strokesWidth}, [](const penrose::PenroseTriangle &tr, size_t) { return !isSmall(tr.color); });
  out << "</g>\n</svg>\n";
  return true;
}

[[nodiscard]] bool saveTiling(const std::string &filename, const std::vector<penrose::PenroseQuadrilateral> &quad, int canvasSize,
                              RGB rgb1, RGB rgb2, RGB background, int threshold) {

  std::ofstream out(filename);
  if (!out) {
    spdlog::error("Cannot open output file : {}.", filename);
    return false;
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 10);

  const float strokesWidth = std::sqrt(normSq(quad[0].vertices[0]-quad[0].vertices[1])) / 30.0f;

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << fmt::format("height='{size}' width='{size}' viewBox='0 0 {size} {size}'>\n", fmt::arg("size",canvasSize))
      << fmt::format("<rect height='100%' width='100%' fill='rgb({},{},{})'/>\n", background.r, background.g, background.b)
      << "<g id='surface1'>\n";
  out << to_path(quad, Fill{rgb1}, {}, [&](const penrose::PenroseQuadrilateral &tr, size_t) { return isSmall(tr.color) ? distrib(gen) >= threshold : false; });
  out << to_path(quad, Fill{rgb2}, {}, [&](const penrose::PenroseQuadrilateral &tr, size_t) { return !isSmall(tr.color) ? distrib(gen) >= threshold : false; });
  out << to_path(quad, {}, StrokesStyle{0, 0, 0, strokesWidth});
  out << "</g>\n</svg>\n";
  return true;
}

[[nodiscard]] bool saveTiling(const std::string &filename,
                              const std::vector<penrose::PenroseQuadrilateral> &quadsStep1,
                              const std::vector<penrose::PenroseQuadrilateral> &quadsStep2,
                              int canvasSize,
                              RGB rgbSmall1, RGB rgbBig1,
                              RGB rgbSmall2, RGB rgbBig2,
                              RGB background, int threshold) {

  std::ofstream out(filename);
  if (!out) {
    spdlog::error("Cannot open output file : {}.", filename);
    return false;
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 10);

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << fmt::format("height='{size}' width='{size}' viewBox='0 0 {size} {size}'>\n", fmt::arg("size", canvasSize))
      << fmt::format("<rect height='100%' width='100%' fill='rgb({},{},{})'/>\n", background.r, background.g, background.b)
      << "<g id='surface1'>\n";

  const float strokesWidthStep2 = norm(quadsStep2[0].vertices[0] - quadsStep2[0].vertices[1]) / 15.0f;
  const float strokesWidthStep1 = norm(quadsStep1[0].vertices[0] - quadsStep1[0].vertices[1]) / 20.0f;

  out << to_path(quadsStep2, Fill{rgbSmall1}, {}, [&](const penrose::PenroseQuadrilateral &tr, size_t) { return isSmall(tr.color) && tr.flag ? distrib(gen) >= threshold : false; });
  out << to_path(quadsStep2, Fill{rgbBig1}, {}, [&](const penrose::PenroseQuadrilateral &tr, size_t) { return !isSmall(tr.color) && tr.flag ? distrib(gen) >= threshold : false; });
  out << to_path(quadsStep2, Fill{rgbSmall2}, {}, [&](const penrose::PenroseQuadrilateral &tr, size_t) { return isSmall(tr.color) && !tr.flag ? distrib(gen) >= threshold : false; });
  out << to_path(quadsStep2, Fill{rgbBig2}, {}, [&](const penrose::PenroseQuadrilateral &tr, size_t) { return !isSmall(tr.color) && !tr.flag ? distrib(gen) >= threshold : false; });
  out << to_path(quadsStep2, {}, StrokesStyle{0, 0, 0, strokesWidthStep2});
  out << to_path(quadsStep1, {}, StrokesStyle{0, 0, 0, strokesWidthStep1});
  out << "</g>\n</svg>\n";
  return true;
}

enum class IsHole : bool { Yes, No };

[[nodiscard]] bool saveTilingNeon(const std::string &filename,
                              const std::vector<penrose::PenroseQuadrilateral> &quadsStep2,
                              int canvasSize,
                              RGB rgbSmall1, RGB rgbBig1,
                              RGB rgbSmall2, RGB rgbBig2,
                              RGB rgbHole, RGB background, int threshold) {

  std::ofstream out(filename);
  if (!out) {
    spdlog::error("Cannot open output file : {}.", filename);
    return false;
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 10);

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << fmt::format("height='{size}' width='{size}' viewBox='0 0 {size} {size}'>\n", fmt::arg("size", canvasSize))
      << fmt::format("<rect height='100%' width='100%' fill='rgb({},{},{})'/>\n", background.r, background.g, background.b)
      << "<g id='surface1'>\n";

  const float strokesWidthStep2 = norm(quadsStep2[0].vertices[0] - quadsStep2[0].vertices[1]) / 45.0f;

  std::vector<IsHole> isHole(quadsStep2.size(), IsHole::No);
  for (auto& tag : isHole)
  {
    tag = distrib(gen) >= threshold ? IsHole::Yes : IsHole::No;
  }
  out << to_path(quadsStep2, {}, StrokesStyle{rgbSmall1, strokesWidthStep2}, [&](const penrose::PenroseQuadrilateral &tr, size_t idx) { return isSmall(tr.color) && tr.flag && isHole[idx] == IsHole::No; });
  out << to_path(quadsStep2, {}, StrokesStyle{rgbBig1, strokesWidthStep2}, [&](const penrose::PenroseQuadrilateral &tr, size_t idx) { return !isSmall(tr.color) && tr.flag && isHole[idx] == IsHole::No; });
  out << to_path(quadsStep2, {}, StrokesStyle{rgbSmall2, strokesWidthStep2}, [&](const penrose::PenroseQuadrilateral &tr, size_t idx) { return isSmall(tr.color) && !tr.flag && isHole[idx] == IsHole::No; });
  out << to_path(quadsStep2, {}, StrokesStyle{rgbBig2, strokesWidthStep2}, [&](const penrose::PenroseQuadrilateral &tr, size_t idx) { return !isSmall(tr.color) && !tr.flag && isHole[idx] == IsHole::No; });

  out << to_path(quadsStep2, {}, StrokesStyle{rgbHole, strokesWidthStep2}, [&](const penrose::PenroseQuadrilateral &tr, size_t idx) { return isHole[idx] == IsHole::Yes; });

  out << "</g>\n</svg>\n";
  return true;
}

[[nodiscard]] bool saveTilingNeon(const std::string &filename,
                              const std::vector<penrose::PenroseQuadrilateral> &quadsStep2,
                              int canvasSize,
                              RGB rgb1, RGB rgb2,
                              RGB background, int threshold) {

  std::ofstream out(filename);
  if (!out) {
    spdlog::error("Cannot open output file : {}.", filename);
    return false;
  }
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 10);

  out << "<svg xmlns='http://www.w3.org/2000/svg' "
      << fmt::format("height='{size}' width='{size}' viewBox='0 0 {size} {size}'>\n", fmt::arg("size", canvasSize))
      << fmt::format("<rect height='100%' width='100%' fill='rgb({},{},{})'/>\n", background.r, background.g, background.b)
      << "<g id='surface1'>\n";

  const float strokesWidthStep2 = norm(quadsStep2[0].vertices[0] - quadsStep2[0].vertices[1]) / 45.0f;

  out << to_path(quadsStep2, {}, StrokesStyle{rgb1, strokesWidthStep2}, [&](const penrose::PenroseQuadrilateral &tr, size_t) { return isSmall(tr.color) ? distrib(gen) >= threshold : false; });
  out << to_path(quadsStep2, {}, StrokesStyle{rgb2, strokesWidthStep2}, [&](const penrose::PenroseQuadrilateral &tr, size_t) { return !isSmall(tr.color) ? distrib(gen) >= threshold : false; });
  out << "</g>\n</svg>\n";
  return true;
}

class Document {
public:
  Document(size_t canvasSize, RGB background) {
    data = fmt::format("<svg xmlns='http://www.w3.org/2000/svg' height='{size}' width='{size}' viewBox='0 0 {size} {size}'>\n"
                      "<rect height='100%' width='100%' fill='{background}'/>\n"
                      "<g id='surface1'>\n",
                      fmt::arg("size", canvasSize),
                      fmt::arg("background", background)
      );
  }

  template <typename T>
  void addPolygon(std::vector<T> polygons, RGB color, StrokesStyle strokeStyle, std::function<bool(const std::type_identity_t<T>&, size_t)> func = [](const T &, size_t){ return true; }) {
    data += fmt::format("<path style='{};{}' d='", color, strokeStyle);
    for (size_t idx = 0; idx < polygons.size(); ++idx) {
      const T& polygon = polygons[idx];
      if (func(polygon, idx))
        data += details::to_path(polygons[idx]) + " ";
    }
    data += "'></path>\n";
  }

  template <typename T>
  void addPolygon(std::vector<T> polygons, RGB color, StrokesStyle strokeStyle, std::function<bool(const std::type_identity_t<T>&)> func) {
    addPolygon(polygons, color, strokeStyle, [](T, size_t){ return func(T); });
  }

  template <typename T>
  void addPolygon(T polygon, RGB color, StrokesStyle strokeStyle) {
    addPolygon(std::vector<T>{polygon}, color, strokeStyle);
  }

  std::string getContent() {
    return data + "</g>\n</svg>\n";
  }

  bool save(const std::string &filename) {
    std::ofstream out(filename);
    if (!out) {
      spdlog::error("Cannot open output file : {}.", filename);
      return false;
    }

    out << getContent();
    return true;
  }

private:
  std::string data;
};

} // namespace svg