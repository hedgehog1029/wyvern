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

void wyvern_file_handle_error(WrenVM* vm) {
    switch (errno) {
        case EBADF:
            wrenSetSlotString(vm, 0, "File is closed!");
            break;
        case EINTR:
            wrenSetSlotString(vm, 0, "System call was interrupted by a signal.");
            break;
        case EIO:
            wrenSetSlotString(vm, 0, "I/O error.");
            break;
        default:
            wrenSetSlotString(vm, 0, "Unknown file error.");
            break;
    }

    wrenAbortFiber(vm, 0);
}

void wyvern_foreign_File_readall(WrenVM* vm) {
    int* fd = (int*) wrenGetSlotForeign(vm, 0);
    char* contents = wyvern_read_all(*fd);

    if (contents == NULL) {
        wyvern_file_handle_error(vm);
        return;
    }

    wrenSetSlotString(vm, 0, contents);
    free(contents);
}

void wyvern_foreign_File_read(WrenVM* vm) {
    int* fd = (int*) wrenGetSlotForeign(vm, 0);
    int n = wrenGetSlotDouble(vm, 1);
    char* contents = wyvern_read(*fd, n);

    if (contents == NULL) {
        wyvern_file_handle_error(vm);
        return;
    }

    wrenSetSlotString(vm, 0, contents);
    free(contents);
}

void wyvern_foreign_File_write(WrenVM* vm) {
    int* fd = (int*) wrenGetSlotForeign(vm, 0);
    const char* contents = wrenGetSlotString(vm, 1);
    int count = wyvern_write(*fd, (char*) contents);

    if (count == -1) {
        wyvern_file_handle_error(vm);
        return;
    }

    wrenSetSlotDouble(vm, 0, count);
}

void wyvern_foreign_File_close(WrenVM* vm) {
    int* fd = (int*) wrenGetSlotForeign(vm, 0);

    int status = wyvern_close_file(*fd);
    if (status == -1) {
        wyvern_file_handle_error(vm);
        return;
    }
}
