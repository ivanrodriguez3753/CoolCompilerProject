class Main {
    someMethod() : Sometype {
        selfDispatch()
    };

    anotherMethod() : Sometype {
        false.dispatch()
    };

    oneMoreMethod() : Sometype {
        false@Sometype.dispatch()
    };
};