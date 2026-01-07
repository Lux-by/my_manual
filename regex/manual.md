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
        regcomp(&regex, pattern, cflags) -- if none flags used, just place '0';

    recomended usage:
        1. this usage helps to check if compilation is succesfully made, otherway it stops the prod:
            
            if (regcomp(&regex, pattern, flags)) {
                printf("Regex compilation failed\n");
                return 1;
            }

Basic regex patterns:

    1. Literal text:
        pattern: "cat";
        match: "cat";

    2. Dot '.' (any single character):
        pattern: "c.t";
        match: "cat", "cut", "c9t" and etc.

    3. Star '*' (0 or more characters, place after character to check):
        pattern: "ca*t";
        match: "ct", "cat", "caaaaat" and etc.

    4. Plus '+' (1 or more characters, place after character to check):
        pattern: "ca+t";
        match: "cat", "caaaaaat". NOT "ct";

    5. Question ? (0 or 1 character, place after character to check):
        pattern: "colou?r";
        match: "color", "colour";

Character classes:

    1. Digits(will check all digits):
        [0-9];
    2. Letters(Will check all letters):
        [a-zA-Z];
    exp:
        [0-9]+ (this find a number in text);

Anchors:
    1. '^' -- start of string;
    2. '$' -- end of string;

    exp:
        ^[0-9]+$ -- this finds matches only numbers;

Capturing matches:
    To extract parts, we use regmatch_t;

    example:
        regex_t regex;
        regmatch_t matches[1];

        regcomp(&regex, "[0-9]+", REG_EXTENDED);

        if (regexec(&regex, "Age: 25", 1, matches, 0) == 0) {
            printf("Found number at %d - %d\n", matches[0].rm_so, matches[0].rm_eo);
        }

    NOTE: rm_so -- is stands for start index, rm_eo -- is stands for end index;

regex_t -- The Regex Object:
    usage:
        regex_t regex; -- creates variable that stored in memory to compile regex machine;

    just note:
        regcomp() -- fill this structure;
        regexec() -- uses it;
        regfree() -- destroys it;

    when regcomp() is used, it returns the value(0-success, !0-fail), you can store the value in variable:
        int ret = regcomp(&regex, pattern, flags);

regexec():
    prototype:
        int regexec(const regex_t *preg, const char *string, size_t nmatch, regmatch_t pmatch[], int eflags);

    preg -- Compiled regex;
    string -- Input text;
    nmatch -- Number of matches you want;
    pmatch -- Array to store match positions;
    eflags -- Execution flags;

    What is regmatch_t type of data:
        it is struct with two type of another struct data types:
            typedef struct {
                regoff_t rm_so; // start index
                regoff_t rm_eo; // end index
            }


