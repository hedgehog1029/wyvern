typedef enum {
    CRITICAL,
    ERROR,
    WARNING,
    INFO,
    FINE,
    DEBUG
} WyvernLogLevel;

char* wyvern_log_level_str(WyvernLogLevel level) {
    switch (level) {
        case CRITICAL: return "CRITICAL";
        case ERROR: return "ERROR";
        case WARNING: return "WARN";
        case INFO: return "INFO";
        case FINE: return "FINE";
        case DEBUG: return "DEBUG";
    }

    return "UNKNOWN";
}

void wyvern_vlog(WyvernLogLevel level, char* message, va_list vargs) {
    printf("[wyvern %s] ", wyvern_log_level_str(level));
    vprintf(message, vargs);
    puts("\0");
}

void wyvern_log(WyvernLogLevel level, char* message, ...) {
    va_list vargs;
    va_start(vargs, message);
    wyvern_vlog(level, message, vargs);
    va_end(vargs);
}

void wyvern_info(char* message, ...) {
    va_list vargs;
    va_start(vargs, message);
    wyvern_vlog(INFO, message, vargs);
    va_end(vargs);
}

void wyvern_warn(char* message, ...) {
    va_list vargs;
    va_start(vargs, message);
    wyvern_vlog(WARNING, message, vargs);
    va_end(vargs);
}
