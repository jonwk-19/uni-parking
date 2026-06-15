# Test Setup

## Install Catch2 (Single Header)

Download the single-header version from:
https://github.com/catchorg/Catch2/releases/download/v3.4.0/catch_amalgamated.hpp

Place it in this directory as `catch.hpp`.

Or run from repo root:
```bash
curl -o tests/catch.hpp https://raw.githubusercontent.com/catchorg/Catch2/devel/extras/catch_amalgamated.hpp
```

## Build and Run Tests

From repo root:
```bash
mkdir build
cd build
cmake .. -DUSAR_MYSQL=OFF  # or ON if MySQL is available
cmake --build .
ctest --verbose
```

## Test Files

- `test_parqueo.cpp` — Core parking lot logic (LIFO/FIFO, capacity, tariff)
- `test_validaciones.cpp` — Input validation (plates, names, capacities, tariffs)

Add more tests for:
- Binary persistence round-trips
- MySQL idempotence
- Billing calculations (fractions, edge cases)
