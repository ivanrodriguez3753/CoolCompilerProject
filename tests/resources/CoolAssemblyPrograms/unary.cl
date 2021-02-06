class Main inherits IO{
    pos : Int <- 5;
    neg : Int <- ~5;
    zero : Int <- 0;
    ttrue : Bool <- true;
    ffalse : Bool <- false;

    main() : Object {{

        if 0 < (~neg) then
            out_string("negation on negative works\n")
        else
            out_string("negation on negative DOES NOT work\n")
        fi;

        if ~pos < 0 then
            out_string("negation on positive works\n")
        else
            out_string("negation on positive DOES NOT work\n")
        fi;

        if ~zero = 0 then
            out_string("negation on zero works\n")
        else
            out_string("negation on zero DOES NOT work\n")
        fi;



        --if not ffalse then
        --    out_string("negation on false works\n")
        --else
        --    out_string("negation on false DOES NOT work\n")
        --fi;

        --if not ttrue then
        --    out_string("negation on true DOES NOT works\n")
        --else
        --    out_string("negation on true works\n")
        --fi;

    }};



};