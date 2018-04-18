typedef enum {
    CRITICAL,
    ERROR,
    WARNING,
    INFO,
    FINE,
    DEBUG
} WyvernLogLevel;

static const char* color_red = "\x1b[31m";
static const char* color_green = "\x1b[32m";
static const char* color_orange = "\x1b[38;5;208m";
static const char* color_blue = "\x1b[36m";
static const char* color_grey = "\x1b[90m";
static const char* color_reset = "\x1b[0m";

char* wyvern_log_level_str(WyvernLogLevel level) {
    switch (level) {
        case CRITICAL: return "critical";
        case ERROR: return "error";
        case WARNING: return "warning";
        case INFO: return "info";
        case FINE: return "good";
        case DEBUG: return "debug";
    }

    return "UNKNOWN";
}

const char* wyvern_log_level_color(WyvernLogLevel level) {
    switch (level) {
        case CRITICAL: return color_red;
        case ERROR: return color_red;
        case WARNING: return color_orange;
        case INFO: return color_blue;
        case FINE: return color_green;
        case DEBUG: return color_grey;
        default: return "";
    }
}

void wyvern_vlog(WyvernLogLevel level, char* message, va_list vargs) {
    const char* color = wyvern_log_level_color(level);

    printf("\x1b[1m%s%s => %s", color, wyvern_log_level_str(level), color_reset);
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
