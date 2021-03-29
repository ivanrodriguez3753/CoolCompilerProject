class A inherits IO{
    method1() : Object {
        out_int(1)
    };
};

class B inherits A {
    method1() : Object {
        out_int(2)
    };
};

class Main {
    main() : Object {{
        let a : A <- new A, b : B <- new B in {
            a.method1();
            b@A.method1();
            b.method1();
        };

    }};
};