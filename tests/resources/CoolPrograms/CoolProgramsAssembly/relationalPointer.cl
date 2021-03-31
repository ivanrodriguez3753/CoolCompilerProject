class Main inherits IO {
    o1 : Object <- new Object;
    o11 : Object <- o1;
    o2 : Object <- new Object;
    i : IO <- new IO;

    main() : Object {{
        --any comparison of different types returns false
        if o1 < i then
            out_string("< does NOT work")
        else
            out_string("< does work")
        fi;
        if o1 <= i then
            out_string("<= does NOT work")
        else
            out_string("<= does work")
        fi;
        if o1 = i then
            out_string("= does NOT work")
        else
            out_string("= does work")
        fi;

        --comparisons of same types, same objects
        if o1 < o1 then -- < defined to be always false
            out_string("< does NOT work")
        else
            out_string("< does work")
        fi;
        if o1 <= o1 then
            out_string("<= does work")
        else
            out_string("<= does NOT work")
        fi;
        if o1 = o1 then
            out_string("= does work")
        else
            out_string("= does NOT work")
        fi;

        --comparisons of same types, diff objects
        if o1 < i then -- < defined to be always false
            out_string("< does NOT work")
        else
            out_string("< does work")
        fi;
        if o1 <= i then
            out_string("<= does NOT work")
        else
            out_string("<= does work")
        fi;
        if o1 = i then
            out_string("<= does NOT work")
        else
            out_string("<= does work")
        fi;
    }};
};