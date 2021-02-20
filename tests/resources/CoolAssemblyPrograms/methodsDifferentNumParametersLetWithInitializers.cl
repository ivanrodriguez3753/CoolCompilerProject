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
        --let x : Int <- 3, y : Int, ab : Int <- 4 in
        --    out_int(x + ab + a + 2); --3 + 4 + 3 + 2 = 12
            --using initialized local vars(2), attribute(1), and literal(1)
        let x : Int, y : Int, z1 : Int <- 1 in
            let x : Int, y : Int, z2 : Int <- a in
                out_int(z1 + z2); --1 + 3 + 3 + 2 = 9
                --using local vars(2) from different scopes, one initialized with an attribute,
                --attribute(1), and literal(1)
                --need to try with a local who has been assigned to

    }};

    main() : Object {{
        method0();
        method1(1);
        method2(1, 2);
        method7(a,a,a,a,a,a,a);
    }};
};