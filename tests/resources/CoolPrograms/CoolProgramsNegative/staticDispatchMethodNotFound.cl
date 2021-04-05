class Main {
    someMethod(x : Int, y : Object, z : Int) : Int {
        5
    };
    main() : Object {
        new Object
    };

    main2(a : Main) : Object {{
        a@Main.out_int(5); --this method is non-existent for this class
    }};
};

