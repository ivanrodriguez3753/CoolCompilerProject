class Main inherits IO {
    small : Int <- 1;
    big : Int <- 100;

    main() : Object {{
        if small < big then {
            out_string("< works");
        }else {
            out_string("< DOES NOT work");
        }fi;

        if small = small then {
            out_string("= works");
        }else {
            out_string("= DOES NOT work");
        }fi;

        if small <= big then {
            out_string("<= works");
        }else {
            out_string("<= DOES NOT work");
        }fi;
    }};
};