class Main inherits IO {
    intAttr : Int <- 3;

    main() : Object {{
        out_int(intAttr);
        intAttr <- 1;
        out_int(intAttr);
        useIntAttr();
    }};

    useIntAttr() : Object {
        out_int(intAttr)
    };
};