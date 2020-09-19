# AVX-FIDDLE
Basic Image Processing algorithms implemented using AVX instructions and benchmarked against non-vectorised version using google benchmark.
## Dependencies
- google/benchmark: https://github.com/google/benchmark
## Installation
```bash
# Clone the project.
$ git clone https://github.com/BhayanakMoth/avx-fiddle.git
# Go to the project root
$ cd avx-fiddle
# Make a build directory
$ cmake -E make_directory "build"
# Generate build system files with cmake.
cmake -DCMAKE_BUILD_TYPE=Release -S . -B "build" -Dbenchmark_DIR="path/to/benchmarkConfig.cmake"
# Build the library.
$ cmake --build "build" --config Release
```
