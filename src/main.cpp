#include <geometry.hpp>
#include <penrose.hpp>

#include <cxxopts.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/cfg/env.h>

#include <fstream>
#include <vector>

int main(int argc, char *argv[]) try {

  spdlog::cfg::load_env_levels();

  // =================================================================================================
  // CLI
  cxxopts::Options options(argv[0], "Description");
  options.positional_help("[optional args]").show_positional_help();

  // clang-format off
  options.add_options()
    ("h,help", "Print help")
    ("l,level", "Number of subdivision done", cxxopts::value<int>())
    ;
  // clang-format on
  options.parse_positional({"level"});
  auto clo = options.parse(argc, argv);

  if (clo.count("help")) {
    std::cout << options.help() << std::endl;
    return EXIT_SUCCESS;
  }

  if (!clo.count("level")) {
    spdlog::error("Deflation level required");
    return EXIT_FAILURE;
  }
  const int level = clo["level"].as<int>();

  // =================================================================================================
  // Code

  using namespace penrose;

  std::vector<PenroseTriangle> tiling;

  // Tiling initialisation
  for (int i = 0; i < 6; ++i) {
    const float phi1 = (2*i - 1) * pi / 10;
    const float phi2 = (2*i + 1) * pi / 10;
    tiling.emplace_back(
      Color::kRed,
      Point(0,0),
      Point(cos(phi1), sin(phi1)),
      Point(cos(phi2), sin(phi2))
    );
  }

  for (auto tr : tiling) {
    fmt::print("{}\n", to_string(tr));
  }
  for (int l = 0; l < level; ++l) {
    tiling = deflate(tiling);
  }

  std::ofstream out("penrose_tiling.svg");
  if (!out) {
      std::cerr << "Cannot open output file.\n";
      return EXIT_FAILURE;
  }

  for (auto tr : tiling) {
    fmt::print("{}\n", to_string(tr));
  }
  fmt::print("Hello World!");


  // out << "<svg xmlns='http://www.w3.org/2000/svg' height='" << r * 16
  //       << "' width='" << r * 16 << "'>\n"
  //       << "<rect height='100%' width='100%' fill='black'/>\n"
  //       << "<g stroke='rgb(255,165,0)'>\n";
  // for (const auto& line : svg)
  //     out << line << '\n';
  // out << "</g>\n</svg>\n";

  return EXIT_SUCCESS;

} catch (const cxxopts::OptionException &e) {
  spdlog::error("Parsing options : {}", e.what());
  return EXIT_FAILURE;

} catch (const std::exception &e) {
  spdlog::error("{}", e.what());
  return EXIT_FAILURE;

}
