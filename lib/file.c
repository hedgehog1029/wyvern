#include <sys/file.h>

static void wyvern_file_handle_error(WrenVM* vm) {
    char* error_str;

    switch (errno) {
        case EACCES:
            error_str = "Access to that file is not allowed (or it does not exist.)";
            break;
        case EBADF:
            error_str = "File is closed!";
            break;
        case EINTR:
            error_str = "System call was interrupted by a signal.";
            break;
        case EIO:
            error_str = "I/O error.";
            break;
        default:
            error_str = "Unknown file error.";
            break;
    }

    wrenSetSlotString(vm, 0, error_str);
    wrenAbortFiber(vm, 0);
}

void wyvern_foreign_File_class_new(WrenVM* vm) {
    int* fd = wrenSetSlotNewForeign(vm, 0, 0, sizeof(int));
    const char* path = wrenGetSlotString(vm, 1);

    *fd = wyvern_open_file(path, O_RDWR);

    if (*fd == -1) {
        wyvern_file_handle_error(vm);
    }
}

void wyvern_foreign_File_class_finalize(void* data) {
    int* fd = (int*) data;
    wyvern_close_file(*fd);
    *fd = -1;

    return;
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

void wyvern_foreign_File_size(WrenVM* vm) {
    int* fd = (int*) wrenGetSlotForeign(vm, 0);
    int size = wyvern_query_size(*fd);

    wrenSetSlotDouble(vm, 0, size);
}

void wyvern_foreign_File_close(WrenVM* vm) {
    int* fd = (int*) wrenGetSlotForeign(vm, 0);

    int status = wyvern_close_file(*fd);
    if (status == -1) {
        wyvern_file_handle_error(vm);
        return;
    }
}
