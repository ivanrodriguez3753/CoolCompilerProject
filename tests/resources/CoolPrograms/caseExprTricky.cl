class Main {
    x : Int <- 5;
    main() : Object {{
        case false of
            x : Main => x;
            x : Int  => x;
            x : IO  => x;
            x : Bool  => x;
            x : String  => x;
            x : Object => x;
        esac;
        x;
    }};
};