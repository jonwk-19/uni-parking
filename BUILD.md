# Build Instructions

## Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.15+
- (Optional) MySQL client library

## Quick Build (no MySQL)

```bash
mkdir build
cd build
cmake ..
cmake --build .
./SistemaParqueo  # Linux/macOS
.\SistemaParqueo.exe  # Windows
```

## Build with MySQL Support

```bash
mkdir build
cd build
cmake .. -DUSAR_MYSQL=ON
cmake --build .
```

**Note:** MySQL client library must be installed:
- **Ubuntu/Debian:** `sudo apt-get install libmysqlclient-dev`
- **macOS:** `brew install mysql-client`
- **Windows:** MSVC project or vcpkg: `vcpkg install mysql:x64-windows`

## Build and Run Tests

```bash
# First, download Catch2 header (from repo root)
curl -o tests/catch.hpp https://raw.githubusercontent.com/catchorg/Catch2/devel/extras/catch_amalgamated.hpp

# Then build
mkdir build
cd build
cmake ..
cmake --build .

# Run tests
ctest --verbose
# or directly
./RunTests  # Linux/macOS
.\RunTests.exe  # Windows
```

## Configuration

Environment variables (when running with `-DUSAR_MYSQL=ON`):
- `PARQUEO_DB_HOST` — MySQL host (default: `localhost`)
- `PARQUEO_DB_USER` — MySQL user (default: `root`)
- `PARQUEO_DB_PASS` — MySQL password (default: empty)
- `PARQUEO_DB_NAME` — Database name (default: `sistema_parqueo`)
- `PARQUEO_DB_PORT` — MySQL port (default: `3306`)

Example:
```bash
export PARQUEO_DB_HOST=192.168.1.100
export PARQUEO_DB_USER=admin
export PARQUEO_DB_PASS=secret
./SistemaParqueo
```

## Old Build Method (g++ directly)

Still works, but CMake is recommended:

**Without MySQL:**
```bash
g++ -o SistemaParqueo main.cpp src/*.cpp -Iinclude -Wall -std=c++17
```

**With MySQL:**
```bash
g++ -o SistemaParqueo main.cpp src/*.cpp -Iinclude -Wall -std=c++17 \
    -DUSAR_MYSQL -lmysqlclient
```

## Clean Build

```bash
rm -rf build/  # or delete build folder on Windows
mkdir build
cd build
cmake ..
cmake --build .
```

## Troubleshooting

### CMake not found
- **Ubuntu/Debian:** `sudo apt-get install cmake`
- **macOS:** `brew install cmake`
- **Windows:** Download from https://cmake.org/download/

### MySQL library not found
- Ensure MySQL client dev package is installed (see Prerequisites)
- On Windows, try: `cmake .. -DMYSQL_LIBRARY=<path> -DMYSQL_INCLUDE_DIR=<path>`

### Tests fail to compile
- Ensure `tests/catch.hpp` is downloaded (see "Build and Run Tests" above)
- Check `tests/SETUP.md` for detailed instructions
