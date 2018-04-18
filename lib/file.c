#include <errno.h>
#include <sys/file.h>

void wyvern_foreign_File_class_new(WrenVM* vm) {
    int* fd = wrenSetSlotNewForeign(vm, 0, 0, sizeof(int));
    const char* path = wrenGetSlotString(vm, 1);

    *fd = wyvern_open_file(path, O_RDWR);
}

void wyvern_foreign_File_class_finalize(void* data) {
    int* fd = (int*) data;
    wyvern_close_file(*fd);
    *fd = 0;

    return;
}

void wyvern_foreign_File_readall(WrenVM* vm) {
    int* fd = (int*) wrenGetSlotForeign(vm, 0);
    char* contents = wyvern_read_all(*fd);

    if (contents == NULL) {
        switch (errno) {
            case EBADF:
                wrenSetSlotString(vm, 0, "File is closed!");
            case EIO:
                wrenSetSlotString(vm, 0, "I/O error.");
            default:
                wrenSetSlotString(vm, 0, "Unknown file error.");
        }

        wrenAbortFiber(vm, 0);
    }

    wrenSetSlotString(vm, 0, contents);
    free(contents);
}

void wyvern_foreign_File_close(WrenVM* vm) {
    int* fd = (int*) wrenGetSlotForeign(vm, 0);

    wyvern_close_file(*fd);
}
