# bg-generation-penrose [![C++ CI](https://github.com/edmBernard/bg-generation-penrose/actions/workflows/cpp.yml/badge.svg)](https://github.com/edmBernard/bg-generation-penrose/actions/workflows/cpp.yml)

Small executable that generate [Penrose Tiling](https://en.wikipedia.org/wiki/Penrose_tiling) and save it in svg.

- Github : [https://github.com/edmBernard/bg-generation-penrose](https://github.com/edmBernard/bg-generation-penrose)

The algorithm use deflation method explain in this 2 articles :
- [https://preshing.com/20110831/penrose-tiling-explained/](https://preshing.com/20110831/penrose-tiling-explained/)
- [https://tartarus.org/~simon/20110412-penrose/penrose.xhtml](https://tartarus.org/~simon/20110412-penrose/penrose.xhtml)


## Dependencies

We use [vcpkg](https://github.com/Microsoft/vcpkg) to manage dependencies

This project depends on:
- [cxxopts](https://github.com/jarro2783/cxxopts): Command line argument parsing
- [fmt](https://fmt.dev/latest/index.html): A modern formatting library
- [spdlog](https://github.com/gabime/spdlog): Very fast, header-only/compiled, C++ logging library


```
./vcpkg install spdlog cxxopts fmt
```

### Compilation

```bash
mkdir build
cd build
# configure make with vcpkg toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake
# on Windows : cmake .. -DCMAKE_TOOLCHAIN_FILE=${env:VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

the server executable is named `bg-generation-penrose`

## Disclaimer

It's a toy project. So if you spot error, improvement comments are welcome.
