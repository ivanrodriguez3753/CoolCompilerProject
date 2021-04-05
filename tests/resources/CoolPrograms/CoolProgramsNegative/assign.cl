class A {

};

class B inherits A {

};

class Main {
    a : A;
    main() : Object {{
        a <- new B;
        a <- false; --invalid
    }};
};