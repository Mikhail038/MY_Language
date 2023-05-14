# Простенький компилятор

**Михаил Р.** _01.2023._

## Сборка


1. Клонируйте мой репозиторий, используя

```
git clone https://github.com/Mikhail038/MY_Language
```

2. Соберите проект
```
make
```
3. После этого запустите фронтенд \
Преобразует код в **абстрактное дерево** _.tr_
```
./front -s FILENAME.mgr
```
Флаг **-s** уберёт вывод логов компилятора в stdout \
Примеры кода есть в папке EXAMPLES/

4. Затем запустите первую версию бэкенда

```
./back_mc
```
Преобразует абстрактное дерево в **псевдо-ассемблерный код** _.asm_, затем в **псевдо-машинный код** _.mc_, a затем запускает его на **псевдо-процесорре** (работает на **C**)

5. А лучше запустите более позднюю версию бэкенда

```
./back_elf
```
Преобразует абстрактное дерево в ассемблерный код (**x86_64**) и записывает его в **ELF** файл, после чего запускает его. ELF файл построен достаточно минималистично и не претендует на использование в больших многомодульных проектах

## Предыстория

Этот проект написан мной во время обучения на 1 курсе МФТИ. Писал я его долго, и это был мой первый такой большой проект, поэтому возможна небольшая невыдержанность кода в одном стиле. Но думаю это не станет большим препятствием для тех, кто захочет найти что-то в моем проекте.

## Основные принципы

Компиляция файла проходит в два этапа: фронтенд (англ. **frontend**) и бэкенд (англ. **backend**).

- ## Frontend

    Эта часть принимает на вход текстовый файл. Затем создает массив токенов с помощью **токенайзера** или **лексического анализатора**. Он разбивает текст из файла на **лексемы** или **токены** - абстрактные единицы, которые далее участвовают в грамматике языка. Лексический анализатор устроен таким образом: в зависимости от символа, он пытается либо продолжить считывать лексему дальше, либо переходит к считыванию следующей лексемы. За работой лексического анализатора можно проследить, если запускать *front* без флага **-s**.

    После этого с массивом токенов работает **синтаксический анализатор**. Он устроен сложнее и, в том числе чтобы облегчить работу ему, часть функций делегирована лексическому анализатору. Синтаксический анализатор проверяет правильность конструкций языка, и если токены поддаются анализу с использованием **грамматики** языка, составляет из них абстрактное дерево (AST tree). Это реализовано с помошью алгоритма **рекурсивного спуска**. Попробую объяснить алгоритм его работы простыми словами.

    Есть набор функций, каждая из которых отвечает за анализ своей грамматической конструкции. В определенном порядке эти функции вызываются: в случае, если более ранняя функция не смогла обнаружить своей граматической конструкции она передает управление следующей, та следующей и так далее. В ходе своей работы, эти функции формируют абстрактное дерево, которое затем записывается в файл.

- ## Backend

    Эта часть принимает на вход абстрактное дерево, и преобразует его в код на ассемблере. Старая версия преобразует в мой псевдо-ассемблер, который выполняется на моем псевдо-процессоре. Более современная версия создаёт корректно запускающийся ELF файл и исполняет его на нативном процессоре.

## Синтаксис
В этом мини-языке на данный момент реализован Си-подобный синтаксис. Исполнение программы начинатеся с головной функции (в текущий реализации *main*). Поддерживается создание циклов (*while*) и условий (*if / else*), возможность объявления и использования своих функций и рекурсии. Есть встроенные функции языка для ввода / вывода (*in / out*). Поддерживается всего один ненулевой тип переменных - double (*var*).

Конкретный вид конструкций языка задается файлом *DSL/Operations.h* который используется для кодогенерации с помощью препроцессора C. Поэтому может легко и быстро изменен.

## Поддерживаемые конструкции языка
- ## Переменные двух типов var / void
Тип void корректно поддерживается разве что в возвращаемом значении у функций, поэтому не рекомендую экспериментировать с ним.
```
...

var a = 1;

var b = 0;

...
```

**Не допускается создание непроинициализированных переменных!**

- ## Арифметические операции, логические операторы и операторы сравнения
Логические операции возвращают **0** в случае *false* и **1** в случае *true*
```
...

var a = 1 + 2 - 3 * ((4 + 5) / 6) ;

var b = a && (7 + 8 < ((9 != 10) || 11));

...
```

- ## Цикл while
```
while (_condition_)
{
    ...
}
```
**Не доспукается использования пустых фигурных скобок { } !**

- ## Условния if / else
```
if (_condition_)
{
    ...
}
else if (_second_condition_)
{
    ...
}
else
{
    ...
}
```
**Не доспукается использования пустых фигурных скобок { } !**

- ## Определение функций и их использование
Исполнение программы начинается с функции *main* \
Рекурсия работает корректно
```
var foo (var _arg_)
{
    ...
}

var bar ()
{
    ...

    foo (_smth_);

    ...
}

var foobar (var _arg_1_, var _arg_2_)
{
    ...
}

...

var main ()
{
    _var_ = foo (_smth_) + bar ();

    ...

    foobar (_smth_1_, _smth_2_);

    ...
}
```
**Определения функций необходимо указывать до их использования!**

- ## Возврат из функций return
```
var foo ()
{
    ...

    return _smth_;
}
```
_Данная кострукция может работать некооректно в функция с возвращаемым занчением типа void_

- ## Функции ввода / вывода
```
...

in (a, b);

...

out (a);

...
```
**Аргументом таких функций должны быть переменные, а не арифметические выражения!**