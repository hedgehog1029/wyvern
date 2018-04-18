#include <stdio.h>
#include <wren.h>
#include <string.h>

#include "core/util.c"
#include "core/log.c"
#include "core/io.c"
#include "core/wren.c"
#include "lib/ffi.c"

void wyvern_print_usage(const char* program_name) {
    printf("Usage:\n\t%s <file.wren>\tCompile and execute a file.\n", program_name);
}

int main(int argc, char const *argv[]) {
    const char* program_name = argv[0];
    const char* main_path = argv[1];

    if (main_path == NULL) {
        wyvern_log(CRITICAL, "No filename was provided!");
        wyvern_print_usage(program_name);
        return 1;
    }

    wyvern_info("Running wren file %s", main_path);
    WrenVM* vm = wyvern_new_vm();
    
    wyvern_import_file(vm, main_path);

    wrenFreeVM(vm);
    return 0;
}
