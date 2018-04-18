#include <sys/file.h>
#include <sys/types.h>
#include <unistd.h>

int wyvern_open_file(const char* path, int flags) {
    return open(path, flags);
}

char* wyvern_read(int fd, int size) {
    char* contents = malloc(size);
    read(fd, contents, size);
    contents[size] = '\0';

    return contents;
}

char* wyvern_read_all(int fd) {
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    return wyvern_read(fd, size);
}
