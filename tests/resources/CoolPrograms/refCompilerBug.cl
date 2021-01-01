class Main {
    main() : Object {
        new Object
    };

    someMethod() : SELF_TYPE {
        (new Main2).someMethod() --returns Main, this is fine
    };
    someMethod2() : SELF_TYPE {
        (new Main3).someMethod() --this returns a Main3, Main3<=Main so this is fine
    };
    someMethodBad() : SELF_TYPE {
        (new Main2).someMethod2() --ERROR: returns a Main2, Main2 !<= Main
    };
};

class Main2 {
    someMethod() : Main {
        new Main
    };
    someMethod2() : SELF_TYPE {
        new SELF_TYPE
    };

};

class Main3 inherits Main {
    someMethod() : SELF_TYPE {
        new SELF_TYPE
    };
};
--Think I found a bug in the ref compiler?
--ERROR: 6: Type-Check: Main does not conform to SELF_TYPE(Main) in method someMethod
