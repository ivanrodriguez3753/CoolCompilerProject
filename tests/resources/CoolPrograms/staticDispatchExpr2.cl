class Base inherits IO {
  x : Int <- 5;
  y : Bool <- true;

  someMethod() : Object {
    let a : Int <- 3 in {
        out_string("Called Base someMethod\n");
    }
  };
};

class Derived inherits Base {

};

class Main {

  main() : Object {
    let base : Base <- new Base, derived : Derived <- new Derived in {
      derived@Derived.someMethod();
    }
  };
};


--The expression type is being determined when we are traversing the _selfDispatch node. Since we are traversing, and we
--came across a _selfDispatch node, then clearly we are within the scope of a class. Actually, we are in the class of


