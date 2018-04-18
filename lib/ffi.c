void wyvern_foreign_File_open_static(WrenVM* vm) {
    wrenEnsureSlots(vm, 3);

    const char* filename = wrenGetSlotString(vm, 1);
    int flags = (int) wrenGetSlotDouble(vm, 2);

    int fd = wyvern_open_file(filename, flags);

    wrenSetSlotDouble(vm, 0, fd);
}

void wyvern_foreign_File_readall_static(WrenVM* vm) {
    int fd = (int) wrenGetSlotDouble(vm, 1);
    char* contents = wyvern_read_all(fd);

    wrenSetSlotString(vm, 0, contents);
    free(contents);
}
