class Main {
    i : Int; --Ints are default initialized so this will not be void
    o : Object; --This object is void since it is not initialized

    main() : Object {{
        case i of
            i : Int => {
                i;
            };
            o : Object => {
                o;
            };
        esac;

        case o of
            i : Int => {
                i;
            };
            o : Object => {
                o;
            };
        esac;
    }};
};