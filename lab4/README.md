# Запуск лабораторной 4

Нужны CMake 3.20+ и компилятор с поддержкой C++20.

## Сборка

```sh
cd /Users/dima-mozhaev/Documents/OOP/lab4
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

Для сборки без тестов добавьте `-DBUILD_TESTING=OFF` к команде конфигурации CMake.

## Запуск программы

```sh
./build/lab4
```

Запуск с готовым примером ввода:

```sh
./build/lab4 < examples/demo.txt
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
