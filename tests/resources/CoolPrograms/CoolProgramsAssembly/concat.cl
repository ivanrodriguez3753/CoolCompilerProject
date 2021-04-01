class Main inherits IO {
    s1 : String <- "abc";
    s2 : String <- "def";

    main() : Object {{

        out_string(s1);
        out_string(s2);
        out_string(s1.concat(s2));
        out_string(s1);
        out_string(s2);
        out_string("".concat(""));
        out_string("".concat("a").concat("b"));
    }};
};