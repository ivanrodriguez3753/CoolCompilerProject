class A {
    i : Int;
};

class B {
    i : Int;
};

class C {
    i : Int;
};

class Main {
    i : Int <- new Int;
    s : String <- new String;
    o : Object <- new Object;
    b : Bool <- new Bool;

    testCase(obj : Object) : Object {
        case obj of
        x : Int =>
            (new IO).out_int(
            1
            );
        x : String =>
            (new IO).out_int(
            2
            );
        x : Object =>
            (new IO).out_int(
            3
            );
        esac
    };

    main() : Object {{
        testCase(i);
        testCase(s);
        testCase(o);
        testCase(b); --note there is no branch for Bools so it will go to the Object branch
        --outputs 1233
    }};
};