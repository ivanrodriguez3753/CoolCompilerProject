class Main inherits IO {
    voidObj1 : Object;
    voidObj2 : Object;
    nonvoidObj : Object <- new Object;

    main() : Object {{
        if voidObj1 < voidObj2 then -- < defined to return false for two void objects
            out_string("< does NOT work")
        else
            out_string("< does work")
        fi;
        if voidObj1 <= voidObj2 then
            out_string("<= does work")
        else
            out_string("<= does NOT work")
        fi;
        if voidObj1 = voidObj2 then
            out_string("= does work")
        else
            out_string("= does NOT work")
        fi;
        --any comparison between a void and nonvoid is false
        --order shouldn't matter so try both orders for all 3 relOP
        if voidObj1 < nonvoidObj then
            out_string("< does NOT work")
        else
            out_string("< does work")
        fi;
        if voidObj1 <= nonvoidObj then
            out_string("<= does NOT work")
        else
            out_string("<= does work")
        fi;
        if voidObj1 = nonvoidObj then
            out_string("= does NOT work")
        else
            out_string("= does work")
        fi;
        --flip the order
        if nonvoidObj < voidObj1 then
            out_string("< does NOT work")
        else
            out_string("< does work")
        fi;
        if nonvoidObj <= voidObj1 then
            out_string("<= does NOT work")
        else
            out_string("<= does work")
        fi;
        if nonvoidObj = voidObj1 then
            out_string("= does NOT work")
        else
            out_string("= does work")
        fi;
    }};
};