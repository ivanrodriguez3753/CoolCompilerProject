class Main inherits IO {
    methodWithBlockExpr() : String {{ --this method will return "truePath"
        false; --dummy expression
        if true then
            "truePath"
        else
            "falsePath"
        fi;
    }};

    main() : Object {
        out_string(methodWithBlockExpr())
    };
};