class Foo {
    intAttr : Int <- 10;
    setIntAttr(x : Int) : Object {
        intAttr <- x
    };
    getIntAttr() : Int {
        intAttr
    };
};
class Main inherits IO {
    intAttr : Int <- 1;
    fooAttr : Foo <- new Foo;

    changeWontBeReflected() : Object {{
        let x : Int <- intAttr in {
            out_int(x); --will print the same as intAttr has, which is 1
            x <- 3; --x and intAttr no longer point at the same Int object, since Ints are immutable
            out_int(x); --will print out 3 (obvious)
            out_int(intAttr); --will print out 1 (not as obvious)
        };
    }};

    changeWillBeReflected() : Object {{
        let x : Foo <- fooAttr in {
            out_int(x.getIntAttr());
            out_int(fooAttr.getIntAttr());
            x.setIntAttr(100);
            out_int(x.getIntAttr());
            out_int(fooAttr.getIntAttr());
        };
    }};

    main() : Object {{
        changeWontBeReflected(); --1 then 3 then 1
        changeWillBeReflected(); --10 then 10 then 100 then 100
        out_int(fooAttr.getIntAttr()); --100 (testing persistence of change)
        --total output is 1 3 1 10 10 100 100 100 (spaces are added to show the different numbers)
    }};
};