class Main inherits IO {
    s : String;

    printStringParam(s : String) : Object {
        out_string(s)
    };
    main() : Object {
        printStringParam("hello world\n")
    };
};