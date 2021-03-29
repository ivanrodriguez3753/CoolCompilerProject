class Main inherits IO {
    i : Int <- 0;
    main() : Object {
        if isvoid
            while i < 5 loop {
                out_int(i);
                i <- i + 1;
            } pool
        then
            out_int(1)
        else
            out_int(0)
        fi
    };
};