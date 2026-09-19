# lab1

A C++17 project built with CMake 3.16 or newer.

## Build and run

Run from this directory:

```sh
cmake -S . -B build
cmake --build build
./build/lab1
```

The `lab1` executable is built from `bin/main.cpp`.

## Unit tests

Tests use Google Test, downloaded during the first CMake configuration
(internet access is required). Run from this directory:

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

To build without tests or downloading Google Test, configure with
`-DBUILD_TESTING=OFF`.
