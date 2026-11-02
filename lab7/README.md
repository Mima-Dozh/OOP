# Запуск лабораторной 7

Нужны CMake 3.20+ и компилятор с поддержкой C++20.

## Сборка

```sh
cd /Users/dima-mozhaev/Documents/OOP/lab7
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

Для сборки без тестов добавьте `-DBUILD_TESTING=OFF` к команде конфигурации CMake.

## Запуск программы

```sh
./build/lab7
./build/lab7 --seconds 2 --seed 17
```

`--seconds` задаёт длительность в секундах (по умолчанию 30).
`--seed` задаёт начальное значение генератора случайных чисел.

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

С ThreadSanitizer, отдельно от ASan:

```sh
cmake -S . -B build-tsan -DENABLE_SANITIZERS=OFF -DCMAKE_BUILD_TYPE=Debug \
  '-DCMAKE_CXX_FLAGS=-fsanitize=thread -fno-omit-frame-pointer' \
  '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=thread'
cmake --build build-tsan --parallel
ctest --test-dir build-tsan --output-on-failure
```
