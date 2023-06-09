﻿# ИДЗ №2
*Шиндяпкин Илья Дмитриевич БПИ-219*
## Задание
### Вариант №32
Шайка пиратов под предводительством Джона Сильвера высадилась на берег Острова
Сокровищ. Не смотря на добытую карту старого Флинта, местоположение
сокровищ по-прежнему остается загадкой, поэтому искать
клад приходится практически на ощупь. Так как Сильвер ходит на
деревянной ноге, то самому бродить по джунглям ему не с руки.
Джон Сильвер поделил остров на участки, а пиратов на небольшие группы.
Каждой группе поручается искать клад на нескольких
участках, а сам Сильвер ждет на берегу. Группа пиратов, обшарив
один участок, переходит на следующий, еще необследованный. За
кончив поиски, пираты возвращаются к Сильверу и докладывают
о результатах. Если какая–то из групп находит клад на одном из
своих участков, она возвращается к Сильверу, который шлет попугая,
инициализирующего прекращение (прервыание) всех работ.
Требуется создать приложение, моделирующее действия
Сильвера и пиратов.

## Задача на 4.
### Общая схема решаемой задачи:
Решение реализовано с использованием:
Множество процессов взаимодействуют с использованием именованных POSIX семафоров. Обмен данными ведется через
разделяемую память в стандарте POSIX.

Количество зон, на которые поделен остров равняется 15.
Семафор позволяет ограничить доступ к каждой из зон, предоставляю доступ лишь одной группе пиратов.
В разделяемой памяти хранится структура,
в которой содержится зона, в которой закопан клад, а также статус для каждой зоны:
- -1 - не исследована
- 0 - исследована и клад не найден
- 1 - исследована и клад найден
Место, где будет закопан клад, выбирается случайно в начале работы программы.
Как только один из пиратов находит клад, он сообщает об этом процессу-капитану, а тот завершает работы всех прочих пиратов.
После этого главный процесс осуществляет очистку разделяемой памяти и семафора. 
Программу можно в любой момент прервать с помощью сочетания CTRL+C, при этом память и семафоры корректно удалятся.
***
### Входные и выходные данные:
Количество пиратов передается как параметр при запуске кода.
Результаты работы процессов выводятся в консоль
***
### Тесты:
В этой же директории есть папка с тестами, где есть скриншоты:
примеры работы программы на различных входных данных.

### Пример компиляции, запуска программы и просмотр результатов:

    $ gcc program.c -o program

	$ ./program 7
***

## Задача на 5.
### Общая схема решаемой задачи:
Решение реализовано с использованием:
Множество процессов взаимодействуют с использованием неименованных POSIX семафоров расположенных в разделяемой
памяти. Обмен данными также ведется через разделяемую память в стандарте POSIX.
***
*Все прочие требования и особенности идентичны тем, что были на оценку 4, и были успешно выполнены.
Программа была протестирована на тех же тестовых данных и показала корректные поведение.*
***

## Часть на 6.
### Общая схема решаемой задачи:
Решение реализовано с использованием:
Множество процессов взаимодействуют с использованием семафоров в стандарте UNIX SYSTEM V.
Обмен данными ведется через разделяемую память в стандарте UNIX SYSTEM V.
***
*Все прочие требования и особенности идентичны тем, что были на оценку 4, и были успешно выполнены.
Программа была протестирована на тех же тестовых данных и показала корректные поведение.*
***

## Задача на 7.
### Общая схема решаемой задачи:
Решение реализовано с использованием:
Множество независимых процессов взаимодействуют с использованием именованных POSIX семафоров.
Обмен данными ведется через разделяемую память в стандарте POSIX.
Теперь решение выполнено, как взаимодействие независимых процессов. Программа разделена на два файла:
captain.c, pirate.c. Процесс капитан подготавливает разделяемую память и семафор, а также очищает их
в конце работы программ. Процессы пираты открывают уже созданные семафор и разделяемую память.
Все прочие функции процессов остались неизменными.
### Пример компиляции, запуска программы и просмотр результатов:
***!Важно компилировать программы именно с такими названиями!***
    $ gcc captain.c -o captain
    $ gcc pirate.c -o pirate
	$ ./captain 7
***
*Все прочие требования и особенности идентичны тем, что были на оценку 4, и были успешно выполнены.
Программа была протестирована на тех же тестовых данных и показала корректные поведение.*
***

## Задача на 8.
### Общая схема решаемой задачи:
Решение реализовано с использованием:
Множество независимых процессов взаимодействуют с использованием семафоров в стандарте UNIX SYSTEM V. Обмен
данными ведется через разделяемую память в стандарте UNIX SYSTEM V.
***
*Все прочие требования и особенности идентичны тем, что были на оценку 7, и были успешно выполнены.
Программа была протестирована на тех же тестовых данных и показала корректные поведение.*
***
