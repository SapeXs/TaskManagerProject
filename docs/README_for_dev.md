# TaskManagerProject

## Code style

C++20-стиль через `clang-format`, `clang-tidy`, `.editorconfig` и CMake-цели.

### Установка инструментов

```bash
sudo apt update
sudo apt install clang-format clang-tidy cmake g++
```

### Автоматическое форматирование

```bash
./scripts/format_cpp.sh fix
```

Старый скрипт тоже работает, вызывая новый:

```bash
./clang_format_script.sh
```

### Проверка форматирования без изменения файлов

```bash
./scripts/format_cpp.sh check
```

или через CMake:

```bash
cmake -S . -B build
cmake --build build --target check-format
```

### Сборка

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j
```

### clang-tidy

```bash
./scripts/tidy_cpp.sh
```

или через CMake:

```bash
cmake --build build --target tidy
```

### Полная локальная проверка

```bash
./scripts/check_cpp_style.sh
```

Эта команда проверяет форматирование, собирает проект и запускает `clang-tidy`.
