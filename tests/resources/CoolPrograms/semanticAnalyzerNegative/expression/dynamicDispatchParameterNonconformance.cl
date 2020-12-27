class Main {
    someMethod(x : Int, y : Object, z : Int) : Int {
        5
    };
    main() : Object {
        new Object
    };

    main2(a : Main) : Object {{
        a.someMethod("notAnInt", 4, "notAnInt"); --wrong parameter types
    }};
};