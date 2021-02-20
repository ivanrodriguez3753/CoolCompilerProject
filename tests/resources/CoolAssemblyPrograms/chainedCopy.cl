class Main inherits IO{
    copy1(copy1Int : Int) : Main {
        out_int(copy1Int)
    };
    copy2(copy2Int : Int) : Main {
        out_int(copy2Int)
    };
    copy3(copy3Int : Int) : Main {
        out_int(copy3Int)
    };
    main() : Object {{
        (new Main)
            .copy()
                .copy1(111)
                    .copy2(222)
                        .copy3(333);
        out_string((0).type_name());
        out_string((new SELF_TYPE).type_name());
    }};
};