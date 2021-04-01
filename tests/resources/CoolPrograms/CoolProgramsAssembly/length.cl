class Main inherits IO {
    s0 : String;
    s1 : String <- "a";
    s3 : String <- "abc";
    s4 : String <- s1.concat(s3);
    s2 : String <- s4.substr(0, 2);

    main() : Object {{
        out_int(s0.length());
        out_int(s1.length());
        out_int(s3.length());
        out_int(s4.length());
        out_int(s2.length()); --outputs 0 1 3 4 2
    }};
};