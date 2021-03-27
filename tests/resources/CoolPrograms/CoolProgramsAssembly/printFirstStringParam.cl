class Main inherits IO {
    s : String;

    printIntParam(s : String, t : String) : Object {
        out_string(s)
    };
    main() : Object {
        printIntParam("hello1\n", "hello2\n")
    };
};