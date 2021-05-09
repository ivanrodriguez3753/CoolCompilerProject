class Main {
    s : String <- "Hello!";
    main() : Object {{
        (new IO).out_string(s.substr(0, 6));
        (new IO).out_string("No error on first substring");
        s.substr(0, 7);
    }};
};