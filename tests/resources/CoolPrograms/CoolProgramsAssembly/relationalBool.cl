class Main inherits IO {
    ffalse : Bool <- false;
    ttrue : Bool <- true;

    main() : Object {{
        if ffalse < ttrue then {
            out_string("< works");
        }else {
            out_string("< DOES NOT work");
        }fi;

        if ffalse = ffalse then {
            out_string("= works");
        }else {
            out_string("= DOES NOT work");
        }fi;

        if ttrue  = ttrue then {
            out_string("= works");
        }else {
            out_string("= DOES NOT work");
        }fi;

        if ffalse <= ttrue then {
            out_string("<= works");
        }else {
            out_string("<= DOES NOT work");
        }fi;
    }};
};