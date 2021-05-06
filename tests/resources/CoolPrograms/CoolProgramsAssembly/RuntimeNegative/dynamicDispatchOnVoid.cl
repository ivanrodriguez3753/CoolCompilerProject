class Main inherits IO {
    nonvoidAttr : Int; --Ints (and Strings/Bools) are default initialized and can never be void
    voidAttr : Object; --This will be void because it has not been initialized

    main() : Object {{
        nonvoidAttr.type_name(); --no error
        out_string(voidAttr.type_name()); --runtime error
    }};
};