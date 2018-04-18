#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int wyvern_open_file(const char* path, int flags) {
    return open(path, flags);
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

int wyvern_query_size(int fd) {
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    return size;
}

char* wyvern_read_all(int fd) {
    int size = wyvern_query_size(fd);
    return wyvern_read(fd, size);
}

int wyvern_write(int fd, char* data) {
    int size = strlen(data);
    int count = write(fd, data, size);

    return count;
}

int wyvern_close_file(int fd) {
    if (fd == -1) {
        errno = EBADF;
        return -1;
    }

    return close(fd);
}

char* wyvern_read_file(const char* path) {
    int fd = wyvern_open_file(path, O_RDONLY);
    if (fd == -1) {
        return NULL;
    }

    char* contents = wyvern_read_all(fd);
    wyvern_close_file(fd);

    return contents;
}
