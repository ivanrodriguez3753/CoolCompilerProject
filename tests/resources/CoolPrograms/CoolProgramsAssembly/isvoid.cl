class Main inherits IO {
    voidObj : Object;
    nonvoidObj : Object <- new Object;

    main() : Object {{
        if isvoid voidObj then
            out_int(1)
        else
            out_int(0)
        fi;
        if isvoid nonvoidObj then
            out_int(0)
        else
            out_int(1)
        fi;
    }};
};