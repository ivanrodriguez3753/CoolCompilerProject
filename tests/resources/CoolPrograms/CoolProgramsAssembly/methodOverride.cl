class A {
    methodA_1() : Object {
        (new IO).
            out_string("A1 defined by A")
    };
    methodA_2() : Object {
        (new IO).
            out_string("A2 defined by A")
    };
};
class B inherits A{
    methodA_1() : Object {
        (new IO).
            out_string("A1 defined by B")
    };
    methodB_1() : Object {
        (new IO).
            out_string("B1 defined by B")
    };
};

class C inherits B {
    methodB_1() : Object {
        (new IO).
            out_string("B1 defined by C")
    };
    methodC_1() : Object {
        (new IO).
            out_string("C1 defined by C")
    };
};


class Main inherits IO {
    a : A <- new A;
    b : B <- new B;
    c : C <- new C;
    main() : Object {{
        let a : A <- new A, b : B <- new B, c : C <- new C in {
            a.methodA_1();
            a.methodA_2();
            b.methodA_1();
            b.methodB_1();
            c.methodB_1();
            c.methodC_1();
        };
    }};
};