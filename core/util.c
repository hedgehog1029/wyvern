char* wyvern_concat(char* a, ...) {
    va_list vargs;
    int size = strlen(a) + 1;
    char* final = malloc(size);
    strcpy(final, a);

    va_start(vargs, a);
    while (true) {
        char* next = va_arg(vargs, char*);
        if (next == NULL) {
            break;
        }

        size += strlen(next);
        final = realloc(final, size);

        strcat(final, next);
    }

    return final;
}

void wyvern_debug_string(char* s) {
    int len = strlen(s);

    printf("string length %i: [", len);
    for (size_t i = 0; i < len; i++) {
        int value = s[i];
        printf("%x", value);

        if (i != len - 1)
            printf(", ");
    }
    puts("]");
}
