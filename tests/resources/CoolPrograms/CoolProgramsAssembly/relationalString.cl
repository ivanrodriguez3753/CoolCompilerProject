class Main inherits IO {
    small : String <- "a";
    big : String <- "b";

    small2 : String <- "aaaaa";
    big2 : String <- "aaaab";

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

        if small2 < big2 then {
            out_string("< works");
        }else {
            out_string("< DOES NOT work");
        }fi;

        if small2 = small2 then {
            out_string("= works");
        }else {
            out_string("= DOES NOT work");
        }fi;

        if small2 <= big2 then {
            out_string("<= works");
        }else {
            out_string("<= DOES NOT work");
        }fi;
    }};
};