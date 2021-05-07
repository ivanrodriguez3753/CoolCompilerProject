class Main {
    willBeZero : Int <- 1;
    main() : Object {{
        1 / willBeZero;
        willBeZero <- willBeZero - 1;
        1 / willBeZero;
    }};
};