class Main {--DO NOT EDIT, HARDCODED TESTCASE
    someMethod(x : Int, y : Object, z : Int) : Int {
        5
    };
    main() : Object {
        new Object
    };

    main2(a : Main) : Object {{
        a.out_int(5); --this method is non-existent for this class
        a.someMethod("notAnInt", 4, "notAnInt"); --wrong parameter types
        a.someMethod(5); --incorrect number of arguments
        a.someMethod("notAnInt", new Object); --wrong parameter types and numArgs

        out_int(5); --this method is non-existent for this class
        someMethod("notAnInt", 4, "notAnInt"); --wrong parameter types
        someMethod(5); --incorrect number of arguments
        someMethod("notAnInt", new Object); --wrong parameter types and numArgs

        (new Int)@Main.someMethod(1, new Object, 1); --Int !<= Main
        a@Main.out_int(5); --this method is non-existent for this class
        a@Main.someMethod("notAnInt", 4, "notAnInt"); --wrong parameter types
        a@Main.someMethod(5); --incorrect number of arguments
        a@Main.someMethod("notAnInt", new Object); --wrong parameter types and numArgs
    }};
};

