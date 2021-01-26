class Main inherits IO{
    a : Int <- 3;
    b : Int <- 4;
    method0() : Int {
        0
    };
    method1(x1 : Int) : Int {
        x1
    };
    method2(x1 : Int, x2 : Int) : Int {
        x1 + x2
    };
    method7(x1 : Int, x2 : Int, x3 : Int, x4 : Int, x5 : Int, x6 : Int, x7 : Int) : Int {
        x1 + x2 + x3 + x4 + x5 + x6 + x7
    };

    main() : Object {{
        out_int(method0());
        out_int(method1(1));
        out_int(method2(1, 2));
        out_int(method7(b,2,3,4,5,6,7));
    }};
};