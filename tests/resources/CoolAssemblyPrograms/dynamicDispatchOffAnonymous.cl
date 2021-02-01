class Main inherits IO {
    ioObj : IO;
    returnNewIO() : IO {
        new IO
    };
    main() : Object {{
        out_int(1);
        (new IO).out_int(1);

    }};
};