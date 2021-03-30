class Dummyclass {

};

class Main inherits IO {
    a1 : Object <- new Object;
    a2 : Object <- new Int;
    a3 : Object <- new String;
    a4 : Object <- new Dummyclass;

    i : Int <- new Int;
    s : String <- new String;
    dc : Dummyclass <- new Dummyclass;

    main() : Object {{
        out_string(a1.type_name());
        out_string(a2.type_name());
        out_string(a3.type_name());
        out_string(a4.type_name());
        out_string(i.type_name());
        out_string(s.type_name());
        out_string(dc.type_name());
    }};
};