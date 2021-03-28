class Main inherits IO {
    main() : Object {{
        if true then
            out_string("this works\n")
        else
            out_string("doesn't work\n")
        fi;
        if false then
            out_string("doesn't work\n")
        else
            out_string("this works\n")
        fi;
    }};
};