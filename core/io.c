#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int wyvern_open_file(const char* path, int flags) {
    int fd = open(path, flags);

    if (fd == -1) {
        wyvern_log(ERROR, "Error opening file! %i", errno);
        return -1;
    }

    return fd;
}

char* wyvern_read(int fd, int size) {
    char* contents = malloc(size);
    int status = read(fd, contents, size);
    if (status == -1) {
        wyvern_log(ERROR, "Error reading file!");
        return NULL;
    }

    contents[size] = '\0';

    return contents;
}

char* wyvern_read_all(int fd) {
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    return wyvern_read(fd, size);
}

void wyvern_close_file(int fd) {
    if (fd == -1) {
        return;
    }

    int status = close(fd);

    if (status == -1) {
        wyvern_log(DEBUG, "Error closing file descriptor %i: %i", fd, errno);
    }
}
