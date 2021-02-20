--This is the example from  https://dijkstra.eecs.umich.edu/eecs483/pa4.php
--Example error report output:
--ERROR: 3: Type-Check: arithmetic on String Int instead of Ints
class Main inherits IO {
 main() : Object {
   out_string("Hello, world.\n" + 16777216) -- adding string + int !?
 } ;
} ;