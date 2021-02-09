class Main inherits IO {
    i : Int <- 0;
    n : Int <- 5;

    check_i() : Bool {{

        i;
        if i < n then {
            out_int(i);
            true;
        } else {

            i;
            false;
        }fi;
    }};

    main() : Object {{
        while check_i() loop {
            i <- i + 1;
        }pool;
    }};
};