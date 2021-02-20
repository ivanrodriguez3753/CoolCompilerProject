class Main {
    someMethod() : Sometype {
        selfDispatch(false)
    };

    anotherMethod() : Sometype {
        false.dispatch(false)
    };

    oneMoreMethod() : Sometype {
        false@Sometype.dispatch(false)
    };
};