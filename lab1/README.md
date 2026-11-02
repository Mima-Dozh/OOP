# Запуск лабораторной 1

Нужны CMake 3.16+ и компилятор с поддержкой C++17.

## Сборка

```sh
cd /Users/dima-mozhaev/Documents/OOP/lab1
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

Для сборки без тестов добавьте `-DBUILD_TESTING=OFF` к команде конфигурации CMake.

## Запуск программы

```sh
./build/lab1
```

## Запуск тестов

```sh
ctest --test-dir build --output-on-failure
```
