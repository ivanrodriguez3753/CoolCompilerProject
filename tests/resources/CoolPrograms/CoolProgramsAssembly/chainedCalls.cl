class Main inherits IO {
    chain(i : Int) : Main {{
        out_int(i);
        self;
    }};
    main() : Object {{
        self.chain(1).chain(2).chain(3).chain(4);


    }};
};