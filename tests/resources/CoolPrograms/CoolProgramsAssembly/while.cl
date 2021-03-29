class Main inherits IO {
    i : Int <- 0;
    main() : Object {
        while i < 5 loop {
            out_int(i);
            i <- i + 1;
        } pool
    };
};