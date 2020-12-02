class Main {
    main() : Object {
        if true then new Object else new Bool fi
    };
    main2() : Object {
        if true then new Bool else new Object fi
    };
};