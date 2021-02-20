class Main inherits IO {
    --This will throw
    --ERROR: 0: Type-Check: class Main method main with 0 parameters not found
    --because main should not have any parameters
    main(obj : Object) : Object {{
        false;
        out_string("Hello, world!\n");
    }};
};