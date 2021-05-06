class Main inherits IO {
    nonvoidAttr : Int; --Ints (and Strings/Bools) are default initialized and can never be void
    voidAttr : Object; --This will be void because it has not been initialized

    main() : Object {{
        nonvoidAttr@Object.type_name();
        out_string(voidAttr@Object.type_name());
    }};
};