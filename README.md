# TaskManagerProject

TaskManagerProject — это консольный менеджер задач для Linux, работающий как пользовательский daemon.

Проект состоит из двух основных программ:

- `taskmanagerd` — daemon, который хранит задачи, обрабатывает команды, делает autosave и отслеживает дедлайны.
- `taskctl` — CLI-клиент для управления задачами через Unix socket.

Проект поддерживает разные типы задач, фильтрацию, изменение задач, сохранение в файл, календарные дедлайны, уведомления через `libnotify` и запуск через `systemd --user`.

## Возможности

- Создание разных типов задач:
  - `reminder`
  - `recurring`
  - `bounded`
  - `savings`
  - `stepped`
  - `final`
- Просмотр всех задач.
- Поиск задачи по `id`.
- Фильтрация по priority, state и tag.
- Вывод всех используемых тегов.
- Изменение существующих задач.
- Сохранение и загрузка задач.
- Поддержка относительного времени: `30s`, `10m`, `2h`, `1d`, `1w`, `3mo`, `1y`.
- Поддержка календарных дедлайнов: `YYYY-MM-DD`, `YYYY-MM-DD HH:MM`, `YYYY-MM-DD HH:MM:SS`.
- Desktop notifications через `libnotify`, если библиотека установлена.
- Запуск daemon-а как пользовательской службы через `systemd --user`.

## Зависимости

Минимальные зависимости:

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config
```

Для уведомлений нужно дополнительно:

```bash
sudo apt install -y libnotify-dev
```

Если `libnotify-dev` не установлен, проект всё равно соберётся, но уведомления будут отключены.

Можно установить зависимости скриптом:

```bash
./scripts/install_deps_ubuntu.sh
```

## Сборка

```bash
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build -j
```

После сборки появятся:

```text
build/taskmanagerd
build/taskctl
```

## Ручной запуск

В первом терминале:

```bash
./build/taskmanagerd
```

Во втором терминале:

```bash
./build/taskctl help
./build/taskctl add reminder "Read book" --time 30m --priority high --tag study
./build/taskctl list
```

Остановить daemon:

```bash
./build/taskctl shutdown
```

## Локальная установка

Для установки в пользовательскую директорию:

```bash
./scripts/install_local.sh
```

Скрипт:

- собирает проект;
- копирует `taskmanagerd` и `taskctl` в `~/.local/bin`;
- копирует systemd unit в `~/.config/systemd/user`;
- включает и запускает `taskmanagerd.service`.

Проверить статус:

```bash
systemctl --user status taskmanagerd.service
```

Посмотреть логи:

```bash
journalctl --user -u taskmanagerd.service -f
```

## Если `taskctl` не найден

Если после установки команда `taskctl` не находится:

```bash
taskctl help
```

и выводится ошибка:

```text
Команда «taskctl» не найдена
```

нужно добавить `~/.local/bin` в `PATH`:

```bash
echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```

Проверка:

```bash
which taskctl
taskctl help
```

Также можно запускать напрямую:

```bash
~/.local/bin/taskctl help
```

## Удаление

Удалить daemon, CLI и systemd unit:

```bash
./scripts/uninstall_local.sh
```

Скрипт не удаляет пользовательские данные.

Данные задач хранятся здесь:

```text
~/.local/share/taskmanager/
```

Удалить данные вручную:

```bash
rm -rf ~/.local/share/taskmanager
```

## Где хранятся данные

Файл задач:

```text
~/.local/share/taskmanager/tasks.txt
```

Unix socket:

```text
$XDG_RUNTIME_DIR/taskmanager.sock
```

Если `XDG_RUNTIME_DIR` не установлен, используется fallback:

```text
/tmp/taskmanager.sock
```

## Основные команды

Показать помощь:

```bash
taskctl help
```

Создать reminder:

```bash
taskctl add reminder "Read book" --time 30m --priority high --tag study
```

Создать reminder с календарным дедлайном:

```bash
taskctl add reminder "Submit report" --at "2026-05-20 18:30" --priority critical
```

Создать reminder на дату без времени:

```bash
taskctl add reminder "Submit report" --date "2026-05-25"
```

Создать recurring task:

```bash
taskctl add recurring "Water plants" --time 1h --interval 1d --tag home
```

Создать bounded recurring task:

```bash
taskctl add bounded "Gym" --time 2h --interval 1w --repeats 10 --tag health
```

Создать savings task:

```bash
taskctl add savings "Buy laptop" --current 10000 --target 70000 --priority critical --tag money
```

Создать stepped task:

```bash
taskctl add stepped "Course project" --step "write code" --step "prepare defense"
```

Создать final deadline task:

```bash
taskctl add final "Course work" --step "write" --step "check" --final "deadline soon"
```

Показать все задачи:

```bash
taskctl list
```

Найти задачу по id:

```bash
taskctl find 1
```

Показать все используемые теги:

```bash
taskctl tags
```

Фильтр по priority:

```bash
taskctl filter priority high
```

Фильтр по state:

```bash
taskctl filter state scheduled
```

Фильтр по tag:

```bash
taskctl filter tag study
```

Изменить title:

```bash
taskctl set-title 1 "Read C++ book"
```

Изменить description:

```bash
taskctl set-description 1 "Important university task"
```

Изменить priority:

```bash
taskctl set-priority 1 critical
```

Добавить tag:

```bash
taskctl add-tag 1 university
```

Удалить tag:

```bash
taskctl remove-tag 1 study
```

Изменить время до дедлайна:

```bash
taskctl set-time 1 2h
```

Изменить точный дедлайн:

```bash
taskctl set-deadline 1 "2026-06-01 23:59"
```

Изменить дедлайн только по дате:

```bash
taskctl set-date 1 "2026-06-01"
```

Изменить interval у recurring task:

```bash
taskctl set-interval 2 1w
```

Изменить количество повторений у bounded recurring task:

```bash
taskctl set-repeats 3 5
```

Добавить значение в savings task:

```bash
taskctl add-value 4 5000
```

Перейти к следующему шагу в stepped/final task:

```bash
taskctl advance 5
```

Пометить stepped/final task как overdue:

```bash
taskctl overdue 5
```

Сбросить recurring task на следующее повторение:

```bash
taskctl reset 2
```

Удалить задачу:

```bash
taskctl remove 1
```

Очистить все задачи:

```bash
taskctl clear
```

Сохранить вручную:

```bash
taskctl save
```

Выключить daemon:

```bash
taskctl shutdown
```

## Форматы времени

Поддерживаются относительные длительности:

```text
30s   = 30 seconds
10m   = 10 minutes
2h    = 2 hours
1d    = 1 day
1w    = 1 week
3mo   = 3 months, approximately 30 days each
1y    = 1 year, approximately 365 days
1h30m = 1 hour 30 minutes
```

Также можно использовать пробелы, но значение нужно брать в кавычки:

```bash
taskctl add reminder "Read book" --time "1h 30m"
```

## Форматы даты и времени

Поддерживаются:

```text
YYYY-MM-DD
YYYY-MM-DD HH:MM
YYYY-MM-DD HH:MM:SS
```

Примеры:

```bash
taskctl add reminder "Read book" --at "2026-05-20 18:30"
taskctl add reminder "Submit report" --date "2026-05-25"
taskctl set-deadline 1 "2026-06-01 23:59"
```

Если передана только дата, время считается как `23:59:59`.

## Systemd commands

Запустить daemon:

```bash
systemctl --user start taskmanagerd.service
```

Остановить daemon:

```bash
systemctl --user stop taskmanagerd.service
```

Перезапустить daemon:

```bash
systemctl --user restart taskmanagerd.service
```

Включить автозапуск:

```bash
systemctl --user enable taskmanagerd.service
```

Выключить автозапуск:

```bash
systemctl --user disable taskmanagerd.service
```

Статус:

```bash
systemctl --user status taskmanagerd.service
```

Логи:

```bash
journalctl --user -u taskmanagerd.service -f
```

## Code style

Форматирование C++ файлов:

```bash
./scripts/format_cpp.sh
```

## Пример полного сценария

```bash
taskctl clear

taskctl add reminder "Read book" --time 30m --priority high --tag study
taskctl add recurring "Water plants" --time 1h --interval 1d --tag home
taskctl add savings "Buy laptop" --current 10000 --target 70000 --priority critical --tag money

taskctl list
taskctl tags
taskctl filter priority high
taskctl set-title 1 "Read advanced C++ book"
taskctl set-time 1 2h
taskctl find 1
taskctl save
```
