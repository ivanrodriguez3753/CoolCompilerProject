class Main { --DO NOT EDIT, HARDCODED TEST CASE
    main() : Object {
        case false of
        x : Bool =>
            false;
        x : Int  =>
            false;
        x : Bool =>
            false; --this breaks because the first case had type Bool as well
        esac
    };
};