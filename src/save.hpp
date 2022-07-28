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
    return os << fmt::format("rgb({},{},{})", rgb.r, rgb.g, rgb.b);
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
    return os << fmt::format("fill:{}", fill.color);
  }
  friend std::ostream &operator<<(std::ostream &os, const std::optional<Fill> &fill ) {
    return os << (fill.has_value() ? fmt::format("{}", fill.value()) : "fill:none");
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

  friend std::ostream &operator<<(std::ostream &os, const StrokesStyle &stroke ) {
    return os << fmt::format("stroke:{};stroke-width:{};stroke-linecap:butt;stroke-linejoin:miter", stroke.color, stroke.width);
  }
  friend std::ostream &operator<<(std::ostream &os, const std::optional<StrokesStyle> &stroke ) {
    return os << (stroke.has_value() ? fmt::format("{}", stroke.value()) : "");
  }
};

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
  void addPolygon(std::vector<T> polygons, std::optional<Fill> color, std::optional<StrokesStyle> strokeStyle, std::function<bool(const std::type_identity_t<T>&, size_t)> func = [](const T &, size_t){ return true; }) {
    data += fmt::format("<path style='{};{}' d='", color, strokeStyle);
    for (size_t idx = 0; idx < polygons.size(); ++idx) {
      const T& polygon = polygons[idx];
      if (func(polygon, idx))
        data += details::to_path(polygons[idx]) + " ";
    }
    data += "'></path>\n";
  }

  template <typename T>
  void addPolygon(std::vector<T> polygons, std::optional<Fill> color, std::optional<StrokesStyle> strokeStyle, std::function<bool(const std::type_identity_t<T>&)> func) {
    addPolygon(polygons, color, strokeStyle, [func](T value, size_t){ return func(value); });
  }

  template <typename T>
  void addPolygon(T polygon, std::optional<Fill> color, std::optional<StrokesStyle> strokeStyle) {
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