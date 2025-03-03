# Simple Bash Utils

## Описание проекта

Этот проект представляет собой реализацию утилит `cat` и `grep` на языке C в рамках учебного проекта. 
Проект имитирует поведение стандартных UNIX-утилит, поддерживает основные ключевые флаги и является POSIX-совместимым.

## Функциональные возможности

### 1. Утилита `s21_cat`

Утилита `s21_cat` позволяет просматривать содержимое текстовых файлов в терминале, аналогично стандартной `cat` в UNIX/Linux. 

**Поддерживаемые флаги:**
- `-b` – нумерует только непустые строки.
- `-e` – добавляет `$` в конце каждой строки и отображает управляющие символы.
- `-n` – нумерует все строки (включая пустые).
- `-s` – сокращает несколько пустых строк подряд до одной.
- `-t` – отображает табуляции как `^I`.
- Поддержка GNU-аналогов: `--number-nonblank`, `--number`, `--squeeze-blank`.

**Пример использования:**
```bash
./src/cat/s21_cat -n file.txt  # Вывести файл с нумерацией строк
```

### 2. Утилита `s21_grep`

Утилита `s21_grep` осуществляет поиск строк, содержащих заданные шаблоны, в файлах или стандартном потоке ввода.

**Поддерживаемые флаги:**
- **Основные:**
  - `-e` – задает регулярное выражение для поиска.
  - `-i` – игнорирует регистр букв.
  - `-v` – инвертирует поиск (показывает строки, не содержащие совпадения).
  - `-c` – выводит количество найденных совпадений вместо строк.
  - `-l` – выводит только имена файлов, содержащих совпадения.
  - `-n` – добавляет номера строк в результатах поиска.
- **Дополнительные:**
  - `-h` – подавляет вывод имен файлов при поиске в нескольких файлах.
  - `-s` – подавляет сообщения об ошибках, например, при отсутствии файла.
  - `-f` – использует файл с шаблонами для поиска.
  - `-o` – выводит только найденные совпадения без остального содержимого строки.
- **Поддержка комбинаций флагов** (например, `-iv`, `-in`).

**Примеры использования:**
```bash
./src/grep/s21_grep -i "hello" file.txt       # Поиск "hello" без учета регистра
./src/grep/s21_grep -v -n "error" log.txt     # Поиск строк без "error" с нумерацией
```

## Установка и сборка

### 1. Клонирование репозитория
```bash
git clone https://github.com/yourusername/simple_bash_utils.git
cd simple_bash_utils
git checkout develop
```

### 2. Компиляция утилит

```bash
make s21_cat   # Сборка утилиты cat
make s21_grep  # Сборка утилиты grep
```

### 3. Запуск тестов (опционально)

```bash
make test
```

## Структура проекта

```
simple_bash_utils/
├── src/
│   ├── cat/           # Исходный код утилиты cat
│   ├── grep/          # Исходный код утилиты grep
│   └── common/        # Общие модули (если используются)
├── tests/             # Набор тестов
├── docs/              # Документация (при наличии)
└── Makefile           # Сценарии сборки
```

## Тестирование

Проект содержит интеграционные тесты, проверяющие все комбинации флагов и сравнивающие их результаты с оригинальными `cat` и `grep`. 

### Запуск тестов:
```bash
make test
```

## Требования для работы
- Компилятор `gcc`
- Утилита `make`
- Библиотека `regex` (обычно предустановлена в Linux, но может потребоваться в macOS)

## Стандарты кодирования

- **Стиль кода**: [Google C Style](https://google.github.io/styleguide/cppguide.html)
- **Стандарт языка**: C11
- **Совместимость**: POSIX.1-2017

## Вклад в проект

Мы приветствуем пул-реквесты и отчеты о проблемах. 
Пожалуйста, убедитесь, что ваш код соответствует стилю проекта и сопровождается соответствующими тестами.

## Лицензия

Проект распространяется под лицензией MIT. Подробности смотрите в файле [LICENSE](LICENSE).
