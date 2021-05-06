class Main inherits IO {
    main() : Object {
        let x : Object <- new Object in {
            case x of
                i : Int => {
                    i;
                };
                s : String => {
                    s;
                };
            esac;
        }
    };
};