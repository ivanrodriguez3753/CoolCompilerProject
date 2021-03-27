class Main inherits IO {
    s : String;

    printStringParam(r : String, s : String) : Object {
        out_string(s)
    };
    main() : Object {
        printStringParam("hello1\n", "hello2\n")
    };
};