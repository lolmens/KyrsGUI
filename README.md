# KyrsGUI
Курсовая работа по программированию

### Prerequisites

Не стоит забывать, что для успешного запуска программы, в дирректории с программой должна находиться папка ConfigAndData, 
в которой должен находитья файл lvls.list, в кодировке Windows-1251. 

```
# <- Комментарий
# Данный файл очень боится ошибок, вплоть до вечных циклов, к составлению подходить стоит с аккуратностью.
#
#lvl_1*idType*name*;
#
-lvl_1*01*Техника*;
#
#lvl_2*idType*idTypeParrent*name*;
#
...
```

## Authors

* **Андрей Проказа** - *Initial work* - [Andrew](https://github.com/lolmens)

## Структура входных данных lvls.list

Для корректного распознавания входных данных следует строго придерживаться следующих правил:
*	Для комментирования используется символ '#', символ должен стоять первым в строке.
*	Все данные в файле делятся на 3 уровня, соответственно в файле должны отсутствовать нарушения логических связей между уровнями
*	Порядок данных находящихся в файле не играет никакой роли, за исключением первой пары метаданных. (ниже более подробно)
*	В документе запрещено использование символов  '\*'   '   '.'  ',' в каких-либо полях
* Для удобства можно пропускать строки между записями, допускается разбитие методаты на строки, следует использовать перенос после символа ',' (см. Общий вид записи третьего уровня). После переноса разрешается использовать *tab* для удобства форматирования.

### Общий вид записи первого уровня
```
-lvl_1*idType*name*;
```
Данную запись можно разделить на функциональные части:
*	*-lvl_1* - Идентификатор уровня, первый уровень рассматривается как категория.
*	*idType* - Ключ для элементов более низкого уровня (2-го и 3-го), используется для осуществления логических связей.
*	*name* - поле содержащее название уровня 
*	*\** - разделитель

Для внесения ясности следует рассмотреть примеры, используемые в работе 
```
#
#lvl_1*idType*name*;
#
-lvl_1*01*Техника*;
-lvl_1*03*Предметы кухонной утвари*;
-lvl_1*05*Животные*;
-lvl_1*15*Персонал*;
-lvl_1*20*Кабинеты*;
#
```
### Общий вид записи второго уровня
```
-lvl_2*idType*idTypeParrent*name*;
```
Где:
*	*-lvl_2* - Идентификатор уровня, второй уровень рассматривается как подкатегория.
*	*idType* - Ключ для элементов более низкого 3-го уровня, используется для осуществления логических связей.
*	*idTypeParrent* - Ключ для поиска родительского элемента 1-го уровня.
*	*name* - поле содержащее название
*	\* - разделитель

Для внесения ясности следует рассмотреть примеры, используемые в работе 
```
#
#lvl_2*idType*idTypeParrent*name*;
#
-lvl_2*06*01*Компьютеры*;
-lvl_2*07*01*Бытовая (и офисная) техника*;
-lvl_2*16*15*Бугалтерия*;
-lvl_2*17*15*IT-отдел*;
-lvl_2*18*15*Отдел Маркетинга*;
-lvl_2*19*15*Разнорабочие и др*;
#
```
### Общий вид записи третьего уровня
```
-lvl_3*idTypeParrent*name*{*key*name*:i:123, *key*name*:S:*Text* ...};
```
Где:
*	*-lvl_3* - Идентификатор уровня, трений уровень рассматривается как непосредственные данные.
*	*idTypeParrent* - Ключ для поиска родительского элемента 1-го уровня.
*	*name* - поле содержащее название уровня 

Далее идёт набор метаданных, заключённый в *{}* скобки, разделяемые ',' символом, метаданные могут быть 2-х видов, содержащие текстовое или численное значение:
```
*key*name*:i:123 - Численное
*key*name*:S:*Text* - Текстовое
```
*	key - ключ, используется для объединения одинаковых типов данных
*	name - Название переменной
*	i или S - Определяют тип переменной текстовый(S) или численный(i)
*	123 или Text - Непосредственно данные


В проекте используются следующие ключи:

| Key  | За что отвечает. |
| ------------- | ------------- |
|0 |	Имя, обычно ФИО|
|1 |	Кабинет, местоположение|
|2 |	Состояние оборудования %|
|3 |	Материнская плата|
|4 |	Сокет|
|5 |	Чипсет|
|6 |	Процессор|
|7 |	Количество памяти|
|8 |	Модель|
|9 |	Пол|

Для внесения ясности следует рассмотреть примеры, используемые в работе 
```
#
#Шаблон для экземпляров 3-го уровня.
#lvl_3*idTypeParrent*name*{*key*name*:i:123, *key*name*:S:*Text* ...}; #i - integer, S - string, key - byte (Если key=0, назначается автоматически) !! Возможно это ещё не реализовано !! 
#
-lvl_3*6*Компьютер*{
	*0*Работник*:S:*Петрова Анна Евгеньевна*,
	*1*Кабинет*:i:101,
	*2*Состояние оборудования %*:i:100,
	*3*Материнская плата*:S:*ASRock H81 Pro BTC R2.0*,
	*4*Сокет*:S:*LGA1150*,
	*5*Чипсет*:S:*Intel H81*,
	*6*Процессор*:S:*Intel Core i5-4460 Haswell (3200MHz LGA1150 L3 6144Kb)*,
	*7*Колличество памяти*:S:*4G*
};
-lvl_3*7*TV*{*8*Модель*:S:*TV Samsung UE43MU6103U*, *1*Кабинет*:S:*Обеденная зона*};
-lvl_3*7*Кофеварка эспрессо*{*8*Модель*:S:*De'Longhi ECAM 22.110*, *1*Кабинет*:S:*Обеденная зона*, *2*Состояние оборудования %*:i:90};
-lvl_3*19*Курьер*{*0*Работник*:S:*Копылов Альфред Оскарович*, *9*Пол*:S:*М*}; 
-lvl_3*20*Бугалтерия*{*1*Кабинет*:i:101};
```
