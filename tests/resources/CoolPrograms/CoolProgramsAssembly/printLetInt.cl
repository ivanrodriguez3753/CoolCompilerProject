class Main inherits IO {
    x : Int <- 3;
    main() : Object {{
        let x : Int <- 4 in
            out_int(x);
        out_int(x);
    }};
};