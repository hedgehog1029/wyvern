#include <dlfcn.h>

void wyvern_import_file(WrenVM* vm, const char* path) {
    int fd = wyvern_open_file(path, 0);
    char* contents = wyvern_read_all(fd);

    WrenInterpretResult result = wrenInterpret(vm, contents);
    free(contents);

    if (result == WREN_RESULT_COMPILE_ERROR) {
        wyvern_log(WARNING, "Error compiling file: %s", path);
    } else if (result == WREN_RESULT_RUNTIME_ERROR) {
        wyvern_log(WARNING, "Error running file: %s", path);
    }
}

char* wyvern_wren_load_module(WrenVM* vm, const char* name) {
    int fd = wyvern_open_file(name, 0);
    char* contents = wyvern_read_all(fd);

    return contents;
}

WrenForeignMethodFn wyvern_wren_bind_foreign_method(WrenVM* vm, const char* module, const char* class_name, bool is_static, const char* signature) {
    void* handle = dlopen(NULL, RTLD_LAZY);
    char static_str[8] = {'\0'};
    char method_name[50];
    char symbol_name[50];

    if (is_static) {
        strcpy(static_str, "_static\0");
    }

    sscanf(signature, "%[^()]", method_name);
    sprintf(symbol_name, "wyvern_foreign_%s_%s%s", class_name, method_name, static_str);
    void* func = dlsym(handle, &symbol_name[0]);

    // wyvern_log(DEBUG, "resolved name: %s", &symbol_name[0]);
    if (func == NULL) {
        wyvern_log(DEBUG, "error loading function: %s", dlerror());
    }

    dlclose(handle);
    return func;
}

WrenForeignClassMethods wyvern_wren_bind_foreign_class(WrenVM* vm, const char* module, const char* class_name) {
    void* handle = dlopen(NULL, RTLD_LAZY);
    char symbol_name_new[50];
    char symbol_name_del[60];
    WrenForeignClassMethods methods;

    sprintf(symbol_name_new, "wyvern_foreign_%s_class_new", class_name);
    sprintf(symbol_name_del, "wyvern_foreign_%s_class_finalize", class_name);

    methods.allocate = dlsym(handle, &symbol_name_new[0]);
    methods.finalize = dlsym(handle, &symbol_name_del[0]);

    dlclose(handle);
    return methods;
}

void wyvern_debug(WrenVM* vm, const char* text) {
    printf("%s", text);
}

void wyvern_report_err(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message) {
    switch (type) {
        case WREN_ERROR_COMPILE:
            wyvern_log(ERROR, "Compile error on line %u in module %s: %s", line, module, message);
            break;
        case WREN_ERROR_RUNTIME:
            wyvern_log(ERROR, "Runtime error: %s\n Stack trace:", message);
            break;
        case WREN_ERROR_STACK_TRACE:
            printf("\t%s (%s @ %u)\n", message, module, line);
            return;
        default:
            break;
    }
}

WrenVM* wyvern_new_vm() {
    WrenConfiguration config;
    wrenInitConfiguration(&config);

    config.loadModuleFn = wyvern_wren_load_module;
    config.bindForeignMethodFn = wyvern_wren_bind_foreign_method;
    config.writeFn = wyvern_debug;
    config.errorFn = wyvern_report_err;

    WrenVM* vm = wrenNewVM(&config);

    return vm;
}
