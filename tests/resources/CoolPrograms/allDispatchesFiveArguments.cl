class Main {
    someMethod() : Sometype {
        selfDispatch(false, false, false, false, false)
    };

    anotherMethod() : Sometype {
        false.dispatch(false, false, false, false, false)
    };

    oneMoreMethod() : Sometype {
        false@Sometype.dispatch(false, false, false, false, false)
    };
};