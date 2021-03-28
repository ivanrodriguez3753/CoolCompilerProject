class Main inherits IO {
    i : Int <- 3;

    main() : Object {{
        useIntParam(4);
        out_int(i);
    }};

    useIntParam(i : Int) : Object {
        out_int(i)
    };
};