class Main {
    someMethod() : Sometype {
        selfDispatch(false, false)
    };

    anotherMethod() : Sometype {
        false.dispatch(false, false)
    };

    oneMoreMethod() : Sometype {
        false@Sometype.dispatch(false, false)
    };
};