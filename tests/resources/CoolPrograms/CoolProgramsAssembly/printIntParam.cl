class Main inherits IO {
    i : Int;

    printIntParam(i : Int) : Object {
        out_int(i)
    };
    main() : Object {
        printIntParam(3)
    };
};