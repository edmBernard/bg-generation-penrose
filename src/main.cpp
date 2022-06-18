//
//  https://github.com/edmBernard/bg-generation-penrose
//
//  Created by Erwan BERNARD on 11/09/2021.
//
//  Copyright (c) 2021 Erwan BERNARD. All rights reserved.
//  Distributed under the Apache License, Version 2.0. (See accompanying
//  file LICENSE or copy at http://www.apache.org/licenses/LICENSE-2.0)
//

#include <geometry.hpp>
#include <penrose.hpp>
#include <save.hpp>

#include <cxxopts.hpp>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <fstream>
#include <vector>
#include <optional>

int main(int argc, char *argv[]) try {

  spdlog::cfg::load_env_levels();

  // =================================================================================================
  // CLI
  cxxopts::Options options(argv[0], "Description");
  options.positional_help("output [level]").show_positional_help();

  // clang-format off
  options.add_options()
    ("h,help", "Print help")
    ("l,level", "Number of subdivision done", cxxopts::value<int>()->default_value("11"))
    ("o,output", "Output filename (.svg)", cxxopts::value<std::string>())
    ("rhombus", "Use Rhombus (P3) form otherwise it use Kite and Dart (P2)", cxxopts::value<bool>())
    ("neon", "Print only the shape border", cxxopts::value<bool>())
    ("step", "Step of the 2 color", cxxopts::value<int>()->default_value("0"))
    ("threshold", "Threshold for holes [0, 10] (0: no holes)", cxxopts::value<int>()->default_value("7"))
    ;
  // clang-format on
  options.parse_positional({"output", "level", "step"});
  auto clo = options.parse(argc, argv);

  if (clo.count("help")) {
    fmt::print("{}", options.help());
    return EXIT_SUCCESS;
  }
  if (!clo.count("output")) {
    spdlog::error("Output filename is required");
    fmt::print("{}", options.help());
    return EXIT_FAILURE;
  }

  const int level = clo["level"].as<int>();
  const int step = clo["step"].as<int>();
  const int threshold = clo["threshold"].as<int>();
  const bool neon = clo["neon"].as<bool>();
  const std::string filename = clo["output"].as<std::string>();

  // =================================================================================================
  // Code

  using namespace penrose;

  auto start_temp = std::chrono::high_resolution_clock::now();

  std::vector<PenroseTriangle> tiling;

  const int canvasSize = 2000;
  const float radius = canvasSize * 0.8f;
  const Point center = canvasSize / 2.f * Point(1, 1);
  // Tiling initialisation
  if (clo.count("rhombus")) {
    for (int i = 0, sign = -1; i < 10; ++i, sign *= -1) {
      const float phi1 = (2 * i - sign) * pi / 10;
      const float phi2 = (2 * i + sign) * pi / 10;

      tiling.emplace_back(
          TriangleKind::kRhombsCyan,
          Point(0, 0) + center,
          radius * Point(cos(phi1), sin(phi1)) + center,
          radius * Point(cos(phi2), sin(phi2)) + center);
    }
  } else {
    for (int i = 0, sign = -1; i < 10; ++i, sign *= -1) {
      const float phi1 = (2 * i - sign) * pi / 10;
      const float phi2 = (2 * i + sign) * pi / 10;

      tiling.emplace_back(
          TriangleKind::kDart,
          radius * Point(cos(phi1), sin(phi1)) + center,
          Point(0, 0) + center,
          radius * Point(cos(phi2), sin(phi2)) + center);
    }
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(0, 10);

  svg::Document doc(canvasSize, svg::RGB{6, 12, 34});

  using Style = std::pair<std::optional<svg::Fill>, std::optional<svg::StrokesStyle>>;

  if (step != 0) {
    std::vector<PenroseQuadrilateral> quadTilingStep1 = deflateAndMerge(tiling, step);
    setRandomFlag(quadTilingStep1, 5);
    tiling = splitShape(quadTilingStep1);

    std::vector<PenroseQuadrilateral> quadTilingStep2 = deflateAndMerge(tiling, level - step);

    Style style1 = {{{26, 78, 196}}, {}};
    Style style2 = {{{16, 48, 120}}, {}};
    Style style3 = {{{20, 145, 239}}, {}};
    Style style4 = {{{13, 98, 162}}, {}};
    Style style5 = {{}, {}};
    const float strokesWidthStep2 = norm(quadTilingStep2[0].vertices[0] - quadTilingStep2[0].vertices[1]) / 15.0f;
    Style style6 = {{}, {{{0, 0, 0}, strokesWidthStep2}}};
    const float strokesWidthStep1 = norm(quadTilingStep1[0].vertices[0] - quadTilingStep1[0].vertices[1]) / 20.0f;
    Style style7 = {{}, {{{0, 0, 0}, strokesWidthStep1}}};

    if (neon) {
      const float margin = std::max(3.f, norm(quadTilingStep2[0].vertices[0] - quadTilingStep2[0].vertices[1]) / 30.0f);

      quadTilingStep2 = addMargin(quadTilingStep2, margin);
      const float strokesWidth = norm(quadTilingStep2[0].vertices[0] - quadTilingStep2[0].vertices[1]) / 30.0f;

      style1 = {{}, {{{175, 231, 245}, strokesWidth}}};
      style2 = {{}, {{{39, 100, 180}, strokesWidth}}};
      style3 = {{}, {{{119, 236, 246}, strokesWidth}}};
      style4 = {{}, {{{175, 231, 245}, strokesWidth}}};
      style5 = {{}, {{{16, 48, 120}, strokesWidth}}};
      style6 = {{}, {}};
      style7 = {{}, {}};
    }

    std::vector<svg::IsHole> isHole(quadTilingStep2.size());
    for (auto& tag : isHole)
    {
      tag = distrib(gen) >= threshold ? svg::IsHole::Yes : svg::IsHole::No;
    }

    doc.addPolygon(quadTilingStep2, style1.first, style1.second, [&](const auto &tr, size_t idx) {
                      return isSmall(tr.color) && tr.flag && isHole[idx] == svg::IsHole::No;
                    });
    doc.addPolygon(quadTilingStep2, style2.first, style2.second, [&](const auto &tr, size_t idx) {
                      return !isSmall(tr.color) && tr.flag && isHole[idx] == svg::IsHole::No;
                    });
    doc.addPolygon(quadTilingStep2, style3.first, style3.second, [&](const auto &tr, size_t idx) {
                      return isSmall(tr.color) && !tr.flag && isHole[idx] == svg::IsHole::No;
                    });
    doc.addPolygon(quadTilingStep2, style4.first, style4.second, [&](const auto &tr, size_t idx) {
                      return !isSmall(tr.color) && !tr.flag && isHole[idx] == svg::IsHole::No;
                    });
    doc.addPolygon(quadTilingStep2, style5.first, style5.second, [&](const auto &tr, size_t idx) {
                      return isHole[idx] == svg::IsHole::Yes;
                    });
    doc.addPolygon(quadTilingStep2, style6.first, style6.second);
    doc.addPolygon(quadTilingStep1, style7.first, style7.second);

  } else {
    std::vector<PenroseQuadrilateral> quadTiling = deflateAndMerge(tiling, level);

    const float strokesWidth = std::sqrt(normSq(quadTiling[0].vertices[0]-quadTiling[0].vertices[1])) / 30.0f;
    Style style1 = {{{140, 140, 140}}, {}};
    Style style2 = {{{70, 70, 70}}, {}};
    Style style3 = {{}, {{{0, 0, 0}, strokesWidth}}};

    if (neon) {
      const float margin = std::max(3.f, norm(quadTiling[0].vertices[0] - quadTiling[0].vertices[1]) / 15.0f);
      quadTiling = addMargin(quadTiling, margin);

      const float strokesWidthMargin = std::sqrt(normSq(quadTiling[0].vertices[0]-quadTiling[0].vertices[1])) / 45.0f;
      style1 = {{}, {{style1.first->color, strokesWidthMargin}}};
      style2 = {{}, {{style2.first->color, strokesWidthMargin}}};
      style3 = {{}, {}};
    }

    doc.addPolygon(quadTiling, style1.first, style1.second, [&](const auto &tr, size_t idx) {
                      return !isSmall(tr.color) && distrib(gen) >= threshold;
                    });
    doc.addPolygon(quadTiling, style2.first, style2.second, [&](const auto &tr, size_t idx) {
                      return isSmall(tr.color) && distrib(gen) >= threshold;
                    });
    doc.addPolygon(quadTiling, style3.first, style3.second);

  }

  doc.save(filename);

  std::chrono::duration<double, std::milli> elapsed_temp = std::chrono::high_resolution_clock::now() - start_temp;
  fmt::print("Execution time: {:.2f} ms \n", elapsed_temp.count());

  return EXIT_SUCCESS;

} catch (const cxxopts::OptionException &e) {
  spdlog::error("Parsing options : {}", e.what());
  return EXIT_FAILURE;

} catch (const std::exception &e) {
  spdlog::error("{}", e.what());
  return EXIT_FAILURE;
}
