class A {
    method1() : Object {
        (new IO).out_int(1)
    };
};

class B inherits A {
    --override the method
    method1() : Object {
        (new IO).out_int(2)
    };
};

class Main inherits IO {
    b : A <- new B;
    a : A <- new A;
    main() : Object {{
        b.method1();
        a.method1();
    }};
};