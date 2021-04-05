class Main {
    actuallyBool : Object <- new Bool;
    main() : Object {
        --note that this is an instance where the type checker makes compromises. more safety -> less 'correctness'
        --in the sense that this is a false positive from the type checker
        if actuallyBool then
            0
        else
            0
        fi
    };
};