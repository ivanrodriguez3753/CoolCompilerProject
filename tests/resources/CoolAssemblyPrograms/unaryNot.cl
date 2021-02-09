class Main inherits IO {
    ffalse : Bool <- false;
    ttrue : Bool <- true;

    main() : Object {{
        if not ffalse then
            out_string("negation on false works\n")
        else
            out_string("negation on false DOES NOT work\n")
        fi;

    }};
};