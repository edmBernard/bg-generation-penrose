#include <geometry.hpp>
#include <penrose.hpp>
#include <save.hpp>

#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>

#include <fstream>
#include <vector>
#include <chrono>

int main(int argc, char *argv[]) try {

  spdlog::cfg::load_env_levels();

  // =================================================================================================
  // CLI
  cxxopts::Options options(argv[0], "Description");
  options.positional_help("[optional args]").show_positional_help();

  // clang-format off
  options.add_options()
    ("h,help", "Print help")
    ("l,level", "Number of subdivision done", cxxopts::value<int>()->default_value("3"))
    ;
  // clang-format on
  options.parse_positional({"level"});
  auto clo = options.parse(argc, argv);

  if (clo.count("help")) {
    std::cout << options.help() << std::endl;
    return EXIT_SUCCESS;
  }

  const int level = clo["level"].as<int>();

  // =================================================================================================
  // Code

  using namespace penrose;

  std::vector<PenroseTriangle> tiling;

  const int canvasSize = 1000;
  const float radius = canvasSize / 2.f;
  // Tiling initialisation
  for (int i = 0, sign = -1; i < 10; ++i, sign *= -1) {
    const float phi1 = (2*i - sign) * pi / 10;
    const float phi2 = (2*i + sign) * pi / 10;

    tiling.emplace_back(
      Color::kRed,
      radius * Point(cos(phi1), sin(phi1)) + radius * Point(1,1),
      Point(0,0) + radius * Point(1,1),
      radius * Point(cos(phi2), sin(phi2)) + radius * Point(1,1)
    );
  }

  for (auto tr : tiling) {
    fmt::print("{}\n", to_string(tr));
  }
  for (int l = 0; l < level; ++l) {
    tiling = deflate(tiling);
  }

  auto start_temp = std::chrono::high_resolution_clock::now();
  if (!svg::saveTiling(tiling, canvasSize)) {
    spdlog::error("Failed to save in file");
    return EXIT_FAILURE;
  }
  std::chrono::duration<double, std::milli> elapsed_temp = std::chrono::high_resolution_clock::now() - start_temp;
  fmt::print("Time to save svg: {:.2f} ms \n", elapsed_temp.count());

  auto start_temp2 = std::chrono::high_resolution_clock::now();
  std::vector<PenroseQuadrilateral> quadTiling = completeShape(tiling);

  // remove duplicate
  std::sort(quadTiling.begin(), quadTiling.end());
  auto last = std::unique(quadTiling.begin(), quadTiling.end());
  quadTiling.erase(last, quadTiling.end());

  if (!svg::saveTiling(quadTiling, canvasSize)) {
    spdlog::error("Failed to save in file");
    return EXIT_FAILURE;
  }
  std::chrono::duration<double, std::milli> elapsed_temp2 = std::chrono::high_resolution_clock::now() - start_temp2;
  fmt::print("Time to save svg: {:.2f} ms \n", elapsed_temp2.count());

  return EXIT_SUCCESS;

} catch (const cxxopts::OptionException &e) {
  spdlog::error("Parsing options : {}", e.what());
  return EXIT_FAILURE;

} catch (const std::exception &e) {
  spdlog::error("{}", e.what());
  return EXIT_FAILURE;

}
