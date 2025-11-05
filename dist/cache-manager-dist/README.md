# cache-manager-dist

Object files (lib) are pre-compiled for Ubuntu 24.04 LTS Docker image. Header
files for API are in include. Makefile is setup to link against header files in
include and object files in lib, with src/main.cpp as the main file. Other
source files are organized in src.  
  
The following template instantiations are provided in cache-manager-lib.o:

```cpp
template class CacheManager<int, int>;
template class CacheManager<int, int, bench::ThreadBench>;
template class CacheManager<int, int, bench::TbbBench>;
template class CacheManager<int, std::string>;
template class CacheManager<int, std::string, bench::ThreadBench>;
template class CacheManager<int, std::string, bench::TbbBench>;
```

Refer to `cache-manager.hpp` for CacheManager API. `benchmark.hpp` is
header-only and provides benchmarking utilities to choose benchmark
implementation (or `NoneBench`) and print/write results.

## Running

1. Install Docker CLI on Linux or Docker Desktop on macOS/Windows.

2.  
   a. On Linux/macOS:

      ```bash
      # builds docker image, runs docker container, and compiles dist with Makefile
      # and runs executable in container
      ./build.sh
      ```

   b. On Windows:  
      Use WSL2 and run build script. OR write your own with PowerShell and refer to the build script.

3. SUCCESS!
