class Main {
    main() : Object {
        new Object
    };

    someMethod() : SELF_TYPE {
        5           --Int !<= SELF_TYPE[Main]
    };              --ERROR: 6: Type-Check: Int does not conform to SELF_TYPE(Main) in method someMethod
};
