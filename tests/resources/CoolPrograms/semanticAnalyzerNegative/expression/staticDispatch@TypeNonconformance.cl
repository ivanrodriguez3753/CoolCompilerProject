class Main {
    someMethod(x : Int, y : Object, z : Int) : Int {
        5
    };
    main() : Object {
        new Object
    };

    main2(a : Main) : Object {{
        (new Int)@Main.someMethod(1, new Object, 1); --Int !<= Main
    }};
};

