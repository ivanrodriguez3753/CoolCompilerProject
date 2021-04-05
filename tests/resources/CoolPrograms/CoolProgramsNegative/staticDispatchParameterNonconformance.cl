class Main {
    someMethod(x : Int, y : Object, z : Int) : Int {
        5
    };
    main() : Object {
        new Object
    };

    main2(a : Main) : Object {{
        a@Main.someMethod("notAnInt", new Object, 4); --wrong parameter types
    }};
};

