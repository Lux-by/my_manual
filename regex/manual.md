header:
    #include <regex.h>

functions:
    regcomp() -- Compile a regex;
    regexec() -- Match regex against text;
    regfree() -- Free memory;
    regerror() -- Get error massage;

regcomp():
    to use this function, we need:&regex, pattern(what we want to search) and flag

    here are some flags to use:
        REG_EXTENDED -- Enables extended regex;
        REG_ICASE -- Ignore case difference;
        REG_NOSUB -- No match positions;
        REG_NEWLINE -- . doesn't match newline;

    usage:
        regcomp(&regex, pattern, flags) -- if none flags used, just place '0';

    recomended usage:
        1. this usage helps to check if compilation is succesfully made, otherway it stops the prod:
            
            if (regcomp(&regex, pattern, flags)) {
                printf("Regex compilation failed\n");
                return 1;
            }
