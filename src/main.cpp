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

  if (step != 0) {
    std::vector<PenroseQuadrilateral> quadTilingStep1 = deflateAndMerge(tiling, step);
    setRandomFlag(quadTilingStep1, 5);
    tiling = splitShape(quadTilingStep1);
    std::vector<PenroseQuadrilateral> quadTilingStep2 = deflateAndMerge(tiling, level - step);

    if (neon) {
      const float margin = std::max(3.f, norm(quadTilingStep2[0].vertices[0] - quadTilingStep2[0].vertices[1]) / 30.0f);

      quadTilingStep2 = addMargin(quadTilingStep2, margin);
      const float strokesWidth = norm(quadTilingStep2[0].vertices[0] - quadTilingStep2[0].vertices[1]) / 45.0f;

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<> distrib(0, 10);
      std::vector<svg::IsHole> isHole(quadTilingStep2.size());
      for (auto& tag : isHole)
      {
        tag = distrib(gen) >= threshold ? svg::IsHole::Yes : svg::IsHole::No;
      }
      svg::Document doc(canvasSize, svg::RGB{6, 12, 34});
      doc.addPolygon(quadTilingStep2,
                     {}, svg::StrokesStyle{svg::RGB{175, 231, 245}, strokesWidth},
                     [&](const penrose::PenroseQuadrilateral &tr, size_t idx) {
                        return isSmall(tr.color) && tr.flag && isHole[idx] == svg::IsHole::No;
                     });
      doc.addPolygon(quadTilingStep2,
                     {}, svg::StrokesStyle{svg::RGB{39, 100, 180}, strokesWidth},
                     [&](const penrose::PenroseQuadrilateral &tr, size_t idx) {
                        return !isSmall(tr.color) && tr.flag && isHole[idx] == svg::IsHole::No;
                     });
      doc.addPolygon(quadTilingStep2,
                     {}, svg::StrokesStyle{svg::RGB{119, 236, 246}, strokesWidth},
                     [&](const penrose::PenroseQuadrilateral &tr, size_t idx) {
                        return isSmall(tr.color) && !tr.flag && isHole[idx] == svg::IsHole::No;
                     });
      doc.addPolygon(quadTilingStep2,
                     {}, svg::StrokesStyle{svg::RGB{76, 142, 240}, strokesWidth},
                     [&](const penrose::PenroseQuadrilateral &tr, size_t idx) {
                        return !isSmall(tr.color) && !tr.flag && isHole[idx] == svg::IsHole::No;
                     });

      doc.addPolygon(quadTilingStep2,
                     {}, svg::StrokesStyle{svg::RGB{16, 48, 120}, strokesWidth},
                     [&](const penrose::PenroseQuadrilateral &tr, size_t idx) {
                        return isHole[idx] == svg::IsHole::Yes;
                     });

      fmt::print(doc.getContent());
      doc.save("toto.svg");

      if (!svg::saveTilingNeon(filename, addMargin(quadTilingStep2, margin), canvasSize,
                               svg::RGB{175, 231, 245}, svg::RGB{39, 100, 180},
                               svg::RGB{119, 236, 246}, svg::RGB{76, 142, 240},
                               svg::RGB{16, 48, 120}, svg::RGB{6, 12, 34}, threshold)) {
        spdlog::error("Failed to save in file");
        return EXIT_FAILURE;
      }
    } else {
      if (!svg::saveTiling(filename, quadTilingStep1, quadTilingStep2, canvasSize,
                           svg::RGB{26, 78, 196}, svg::RGB{16, 48, 120},
                           svg::RGB{20, 145, 239}, svg::RGB{13, 98, 162},
                           svg::RGB{6, 12, 34}, threshold)) {
        spdlog::error("Failed to save in file");
        return EXIT_FAILURE;
      }
    }

  } else {
    std::vector<PenroseQuadrilateral> quadTiling = deflateAndMerge(tiling, level);

    if (neon) {
      const float margin = std::max(3.f, norm(quadTiling[0].vertices[0] - quadTiling[0].vertices[1]) / 15.0f);
      if (!svg::saveTilingNeon(filename, addMargin(quadTiling, margin), canvasSize, svg::RGB{140, 140, 140}, svg::RGB{70, 70, 70}, svg::RGB{30, 30, 30}, threshold)) {
        spdlog::error("Failed to save in file");
        return EXIT_FAILURE;
      }
    } else {
      if (!svg::saveTiling(filename, addMargin(quadTiling, 15.f), canvasSize, svg::RGB{140, 140, 140}, svg::RGB{70, 70, 70}, svg::RGB{30, 30, 30}, threshold)) {
        spdlog::error("Failed to save in file");
        return EXIT_FAILURE;
      }
    }
  }
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
