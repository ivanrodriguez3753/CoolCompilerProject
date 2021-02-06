class Main inherits IO{
    source : Int <- 1;
    dest : Int;

    main() : Object {{
        dest <- 2;
        if dest = 2 then
            out_string("assigning Int literal works\n")
        else
            out_string("assigning Int literal DOES NOT work\n")
        fi;

        dest <- source;
        if dest = 1 then
            out_string("assigning Int from attribute identifier works\n")
        else
            out_string("assigning Int from attribute identifier DOES NOT work\n")
        fi;
    }};
};