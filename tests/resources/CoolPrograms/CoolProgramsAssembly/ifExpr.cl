class Main inherits IO {
    main() : Object {{
        if true then
            out_string("this works")
        else
            out_string("doesn't work")
        fi;
        if false then
            out_string("doesn't work")
        else
            out_string("this works")
        fi;
    }};
};