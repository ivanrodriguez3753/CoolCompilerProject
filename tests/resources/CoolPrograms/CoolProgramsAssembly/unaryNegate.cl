class Main inherits IO{
    pos : Int <- 5;
    neg : Int <- ~5;
    zero : Int <- 0;
    ttrue : Bool <- true;
    ffalse : Bool <- false;

    main() : Object {{
        if 0 < (~neg) then
            out_int(0)
        else
            out_int(1)
        fi;
        if ~pos < 0 then
            out_int(2)
        else
            out_int(3)
        fi;
        if ~zero = 0 then
            out_int(4)
        else
            out_int(5)
        fi;
    }};
};