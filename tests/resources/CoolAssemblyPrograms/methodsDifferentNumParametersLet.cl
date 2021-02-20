class Main inherits IO{
    a : Int <- 3;

    method0() : Object {
        out_int(0)
    };
    method1(x1 : Int) : Object {
        out_int(x1)
    };
    method2(x1 : Int, x2 : Int) : Object {
        out_int(x1 + x2)
    };
    method7(x1 : Int, x2 : Int, x3 : Int, x4 : Int, x5 : Int, x6 : Int, x7 : Int) : Object {{
        let x : Int, y : Int, z : Int, x1 : Int, y1 : Int, z1 : Int in
            let x : Int, y : Int, z : Int in
                out_int(999);
        let x : Int, y : Int, z : Int, ab : Int in
           out_int(999);
    }};

    main() : Object {{
        method0();
        method1(1);
        method2(1, 2);
        method7(a,a,a,a,a,a,a);
    }};
};