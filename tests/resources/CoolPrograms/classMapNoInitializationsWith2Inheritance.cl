class Main inherits Parent {
    firstAttr_intMain : Int;
    secondAttr_stringMain : String;

    main() : Object {
        false
    };
};

class Parent inherits Grandparent{
    firstAttr_intParent : Int;
    secondAttr_stringParent : String;

    someMethod() : Bool {
        false
    };
};

class Grandparent {
    anAttr : Int;
    anotherOne : Bool;
};