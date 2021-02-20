class Main {
    main() : Object {
        new Object
    };

    main2(a : Main) : Object {{
        a.out_int(5); --this method is non-existent for this class
    }};
};