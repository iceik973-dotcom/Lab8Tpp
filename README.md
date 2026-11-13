# Лабораторна робота №8 — Варіант 6

**Тема:** Робота з базами даних через ORM (C++20).

**Предметна область:** Система керування чергою друку. Основна сутність — користувач (`User`), залежна — документ друку (`PrintDocument`). Зв'язок 1:N реалізовано зовнішнім ключем `user_id`.

## Склад проєкту

- `include/Models.h` — моделі користувача та документа.
- `include/Database.h` — відображення SQLite через sqlite_orm, дві таблиці та зовнішній ключ.
- `include/Repository.h` — інтерфейс `IUserRepository`, реалізація `UserRepository` та доступ до документів.
- `include/RawSql.h`, `src/RawSql.cpp` — порівняльні операції SQLite C API із prepared statements.
- `src/main.cpp` — демонстрація CRUD, вибірок, JOIN, транзакцій і rollback.
- `docs/` — Word-звіт та опис архітектури.
- `CMakeLists.txt`, `CMakePresets.json`, `vcpkg.json` — конфігурація збірки та залежності.

## Архітектура

```text
main.cpp
  |-- IUserRepository --> UserRepository --> sqlite_orm --> SQLite
  |-- DocumentRepository -------------------> sqlite_orm --> SQLite
  |-- RawSql.cpp ---------------------------> SQLite C API
  `-- Database.h --> Users (1) ---- (N) PrintDocuments
```

## Вимоги

Visual Studio з підтримкою C++, CMake 3.21+, C++20, Ninja та vcpkg. Пакети: `sqlite3`, `sqlite-orm`. Google Test і SFML не потрібні.

## Збирання у Visual Studio

1. Установіть компоненти **Desktop development with C++**, CMake і Ninja.
2. Установіть vcpkg і задайте змінну середовища `VCPKG_ROOT` до його кореневої папки.
3. Відкрийте кореневу папку проєкту через **File → Open → Folder**.
4. Оберіть preset `windows-vcpkg` та дочекайтеся встановлення залежностей.
5. Зберіть і запустіть ціль `PrintQueueORM`.

Альтернативно в Developer PowerShell:

```powershell
cmake --preset windows-vcpkg
cmake --build --preset windows-vcpkg
ctest --preset windows-vcpkg
```

Якщо `VCPKG_ROOT` не задано, укажіть повний шлях до `vcpkg.cmake` через `-DCMAKE_TOOLCHAIN_FILE=...` або відредагуйте preset.

## Сценарії демонстрації

1. Створення 5 користувачів і 15 документів.
2. Порівняння INSERT/SELECT через SQLite C API та sqlite_orm.
3. CRUD через інтерфейс Repository.
4. Фільтрація, сортування з LIMIT, SUM і JOIN.
5. Успішна транзакція з додаванням користувача та документа.
6. Помилка зовнішнього ключа і перевірка відкату обох змін.

База `:memory:` створюється заново для кожного запуску, тому тестові записи не накопичуються.

## GitHub

Папки `build/`, `.vs/` та тимчасові файли не завантажувати; вони виключені в `.gitignore`.


