# Архітектура

Основна сутність `User` має поля id, name, department, status. Залежна `PrintDocument` має id, userId, title, pages, status, cost. Зовнішній ключ `PrintDocuments.user_id` посилається на `Users.id`.

`Database.h` описує схему засобами `make_storage`, `make_table`, `make_column` і `foreign_key`. `UserRepository` реалізує CRUD через інтерфейс `IUserRepository`. `DocumentRepository` керує залежною сутністю. `RawSql.cpp` містить тільки порівняльні INSERT і SELECT з SQLite C API.

Демонстраційні дані зберігаються у базі `:memory:`. Транзакції змінюють обидві таблиці. У сценарії помилки друга вставка порушує зовнішній ключ, тому SQLite відкочує і першу вставку.
