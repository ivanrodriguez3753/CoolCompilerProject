class Main inherits IO {
    ffalse : Bool <- false;
    ttrue : Bool <- true;

    main() : Object {{
        if not ffalse then
            out_int(0)
        else
            out_int(1)
        fi;
        if not ttrue then
            out_int(2)
        else
            out_int(3)
        fi;
    }};
};