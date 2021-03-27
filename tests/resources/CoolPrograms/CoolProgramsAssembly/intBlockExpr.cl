class Main inherits IO {
    main() : Object {
        out_int(
            {   --technically this is a single expression so it it will
            1;  --just pass the value of the last expression to out_int
            2;
            3;
            4;
            }
        )
    };
};