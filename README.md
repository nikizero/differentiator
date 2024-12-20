# О проекте

Данный проект реализует автоматическое дифференцирование математических выражений, их оптимизацию и формирование результата в формате LaTeX. Он позволяет:

- Прочитать из файла математическое выражение.
- Разобрать его в дерево разбора (AST).
- Выполнить дифференцирование по выбранной переменной.
- Оптимизировать результат, упростить выражение.
- Вывести итог в файле формата `.tex` с оформлением в LaTeX.
- Дополнительно разложить исходную функцию в ряд Тейлора вокруг заданной точки.

## Структура кода

- **tree.cpp / tree.h**:  
  Реализуют основные структуры данных для дерева разбора, функции для создания, удаления, оптимизации дерева, а также вспомогательные функции вроде `Eval`, `ContainsVar`, `FindVariable` и др.

- **differentiator.cpp / differentiator.h**:  
  Содержат логику дифференцирования выражений по заданной переменной.

- **io.cpp / io.h**:  
  Отвечают за ввод-вывод данных: чтение исходного выражения из файла `input.txt`, запрос переменной дифференцирования и точки разложения в ряд Тейлора. Также здесь можно определить функцию `FileInput` для чтения входных данных.

- **tex.cpp / tex.h**:  
  Предоставляют функции для генерации TeX-вывода, оформляют исходную функцию, производную и оптимизации в удобочитаемый LaTeX-документ.

- **main.cpp**:  
  Точка входа в программу. Здесь происходят вызовы основных функций: инициализация контекста, чтение выражения, дифференцирование, оптимизация, разложение в ряд Тейлора, а также формирование итогового `.tex` файла.

## Замечания

В репозитории есть скомпилированные примеры взятия производной и разложения. Результать работы выводить в out.tex Ввод через input.txt
