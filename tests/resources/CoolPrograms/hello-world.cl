class Main inherits IO {
  main() : Object {{
    out_string("Hello, world.\n");
    (new Main2).main();
  }} ;
} ;

class Main2 inherits IO {
  main() : Object {
    (new IO).out_string("Hello, world.\n")
  } ;
} ;

