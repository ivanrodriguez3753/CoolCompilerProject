class Main inherits IO{
    a : Int <- 3;

    main() : Object {{
        let x : Int <- a in {
            out_int(x);
            out_int(a);
        };

    }};
};