class Main inherits IO {
    s1 : String <- "abcdef";
    main() : Object {{
        out_string(s1.substr(1, 3)); --bcd

    }};
};