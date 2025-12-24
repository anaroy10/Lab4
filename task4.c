#include <stdio.h>
#include <stdlib.h>

// Count the number of digit characters ('0'–'9') in the input string
int count_digits(const char *s) {
    int n = 0;
    for (; *s; s++) {
        if (*s >= '0' && *s <= '9') {
            n++;
        }
    }
    return n;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int result = count_digits(argv[1]);
    printf("%d\n", result);
    return EXIT_SUCCESS;
}
