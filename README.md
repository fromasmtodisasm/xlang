This is training interpreter for x language

GRAMMAR:
-----------------------------------

**Lexeme type**:
 
1) Keywords
var, int, float, void, begin, end, if, then, else, while, do, write, return, read, waserror, exit

2) Identifier
Начинаются с буквы или “_”

3) Операции почти как в плюсах:
=, ||, &&, +, -, *, /, %, ^ (возведение в степень), !, ==, !=, <, <=, >, >=

4) Разделители:
; , ( ) EOF

5) Литерал численный:
число либо число с точкой.

6) Литерал строковый:
набор символов между “”, \n – новая строка, \t - табуляция

{} – комментарии.

Переменные могут быть глобальными и локальными. Блок “begin”…”end” не 
локализует переменные,описанные в нем. Локальные переменные
скрывают глобальные.

Выполнение начинается с функции int main(…), она всегда должна присутствовать.

Если описание функции заканчивается “;” а не блоком “begin”…”end” – это упреждающее описание, тело этой функции должно быть объявлено где-то дальше по тексту программы.

“else” относится к ближайшему “if”.

Нетерминалы – слова без кавычек,
Терминалы – слова в кавычках.
{“a”} – повторение “a”  0 или более раз,
[a|b|c] – альтернатива между a,b или c
[a]? – a встречается 0 или 1 раз

I – идентификатор,
L – численный литерал,
Ls – строковый литерал

* Program -> {[VarDef|Func]}EOF
* VarDef -> “var” [“int”, ”float” ]  I {“,” I}
* Func -> [“void”, “int”, “float” ] I “(“ [ [“int”, “float”] I {“;” [“int”, “float”] I } ]? “)” [“;”| BL]
* BL -> “begin” ST {“;” ST} “end”
* ST -> [
*    I “=” E | VarDef |  
*    “if” E “then” ST [“else” ST]? |
*    “while” E “do” ST |
*    BL |
*   “return” E |
*    “write” “(“  [E|Ls] {“,” [E|Ls] } “)”  |
*    “read” I |
*    “exit” E |
*    I “(“ [ E {“,” E} ]? “)”
*    ]
* E -> E1 { “||” E1 }
* E1 -> E2 { “&&” E2 }
* E2  -> E7 [ [“==”, ”!=”]  E7 ]?
* E7 -> E3 [  [“>”, ”>=”, ”<”, ”<=”] E3 ]?
* E3 -> E4 { [“+”, ”-”] E4}
* E4 -> E5 { [“*”, ”/”, ”%”]  E5 }
* E5 -> E6 [ “^” E6 ]?
* E6 ->   “waserror” |
*    L |
*    !E6 |
*    -E6 |
*    +E6|
*    (E) |
*    I |
*    I “(“ [ E {“,” E} ]? “)”


BUILD:
-----------------------------------

cd /path/to/xlang/
cmake -T <toolset-name> -S <path-to-source> -B <path-to-build>
<path-to-build> build directory
<toolset-name> toolset name (visual studio, makefile e.t.c)
<path-to-source> location of CMakeList.txt

