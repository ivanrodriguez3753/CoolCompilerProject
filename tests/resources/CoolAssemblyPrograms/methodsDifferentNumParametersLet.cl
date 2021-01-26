class Main {
    a : Int <- 3;

    method0() : Object {
        new Object
    };
    method1(x1 : Int) : Object {
        new Object
    };
    method2(x1 : Int, x2 : Int) : Object {
        new Object
    };
    method7(x1 : Int, x2 : Int, x3 : Int, x4 : Int, x5 : Int, x6 : Int, x7 : Int) : Object {{
        let x : Int, y : Int, z : Int in
            let x : Int, y : Int, z : Int in
                false;
        let x : Int, y : Int, z : Int, ab : Int in
            false;
    }};

    main() : Object {{
        method0();
        method1(1);
        method2(1, 2);
        method7(a,a,a,a,a,a,a);
    }};
};