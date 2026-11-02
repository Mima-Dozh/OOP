# Запуск лабораторной 5

Нужны CMake 3.20+ и компилятор с поддержкой C++20.

## Сборка

```sh
cd /Users/dima-mozhaev/Documents/OOP/lab5
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

Для сборки без тестов добавьте `-DBUILD_TESTING=OFF` к команде конфигурации CMake.

## Запуск программы

```sh
./build/lab5
```

## Запуск тестов

```sh
ctest --test-dir build --output-on-failure
```

С AddressSanitizer и UndefinedBehaviorSanitizer:

```sh
cmake -S . -B build-asan -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-asan --parallel
ctest --test-dir build-asan --output-on-failure
```

Проверка утечек памяти на macOS без ASan:

```sh
cmake -S . -B build -DENABLE_SANITIZERS=OFF -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
MallocStackLogging=1 leaks --atExit -- ./build/lab5
MallocStackLogging=1 leaks --atExit -- ./build/lab5_tests
```
