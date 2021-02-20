class Main inherits IO {
    i : Int <- 0; --redundant but whatever
    n : Int <- 5;

    main() : Object {{
        while i < n loop {
            out_int(i);
            i <- i + 1;
            i;
        }pool;
    }};
};