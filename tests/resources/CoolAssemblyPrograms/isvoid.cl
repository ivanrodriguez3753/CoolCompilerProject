class Main inherits IO {
    isvoidObj : Object;
    isntvoidObj : Object <- new Object;
    isvoidObj2 : Object;
    isntvoidObj2 : Object <- new Object;
    str : String;
    strInit : String <- new String;
    int : Int;
    intInit : Int <- new Int;
    bool : Bool;
    boolInit : Bool <- new Bool;

    checkVoid(o : Object) : Object {{
        if isvoid o then {
            out_string("it was void\n");
        }
        else {
            out_string("it wasnt void\n");
        } fi;
    }};

    main() : Object {{
        checkVoid(isvoidObj);
        checkVoid(isntvoidObj);

        if (isvoid isvoidObj) then {
            out_string("isvoidObj was in fact void\n");
        }
        else {
            out_string("isvoidObj was NOT void... something is wrong\n");
        }fi;

    }};
};