#include <stdio.h>
#include <regex.h>

int main() {
	regex_t regex;
	const char *pattern = "Andy";
	const char *text = "hello world";

	if (regcomp(&regex, pattern, 0)) {
		printf("Regex compilation failed\n");
		return 1;
	}

	if (regexec(&regex, text, 0, NULL, 0) == 0) {
		printf("Match found!\n");
	} else {
		printf("No match\n");
	}

	regfree(&regex);
	return 0;
}
