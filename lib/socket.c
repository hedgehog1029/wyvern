#include <sys/socket.h>
#include <netdb.h>

typedef struct {
    int fd;
    struct addrinfo* rp;
} wyvern_socket_info;

int wyvern_socket_get_protocol(const char* name) {
    if (strcmp(name, "tcp") == 0) {
        return SOCK_STREAM;
    } else if (strcmp(name, "udp") == 0) {
        return SOCK_DGRAM;
    } else {
        return SOCK_RAW;
    }
}

void wyvern_socket_handle_error(WrenVM* vm, const char* error) {
    wrenSetSlotString(vm, 0, error);
    wrenAbortFiber(vm, 0);
}

void wyvern_foreign_Socket_class_new(WrenVM* vm) {
    wyvern_socket_info* info = wrenSetSlotNewForeign(vm, 0, 0, sizeof(wyvern_socket_info));
    const char* protocol_str = wrenGetSlotString(vm, 1);
    const char* uri = wrenGetSlotString(vm, 2);
    const char* port = wrenGetSlotString(vm, 3);

    int protocol = wyvern_socket_get_protocol(protocol_str);
    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = protocol;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    int s = getaddrinfo(uri, port, &hints, &result);
    if (s != 0) {
        wyvern_socket_handle_error(vm, gai_strerror(s));
        return;
    }

    info->fd = -1;
    info->rp = result;
}

void wyvern_foreign_Socket_class_finalize(void* data) {
    wyvern_socket_info* info = (wyvern_socket_info*) data;

    if (info->fd == -1)
        return;

    close(info->fd);
    info->fd = -1;

    freeaddrinfo(info->rp);
}

void wyvern_foreign_Socket_connect(WrenVM* vm) {
    wyvern_socket_info* info = (wyvern_socket_info*) wrenGetSlotForeign(vm, 0);
    struct addrinfo* rd;

    for (rd = info->rp; rd != NULL; rd = rd->ai_next) {
        info->fd = socket(rd->ai_family, rd->ai_socktype, rd->ai_protocol);
        if (info->fd == -1)
            continue;

        if (connect(info->fd, rd->ai_addr, rd->ai_addrlen) != -1)
            break;

        close(info->fd);
    }

    if (rd == NULL) {
        wyvern_socket_handle_error(vm, "Could not connect to socket.");
        info->fd = -1;
        return;
    }
}

void wyvern_foreign_Socket_read(WrenVM* vm) {
    wyvern_socket_info* info = (wyvern_socket_info*) wrenGetSlotForeign(vm, 0);
    if (info->fd == -1) {
        wyvern_socket_handle_error(vm, "Socket is not connected!");
        return;
    }

    int size = (int) wrenGetSlotDouble(vm, 1);
    char* buf = malloc(size);
    size_t rd = read(info->fd, buf, size);

    wrenSetSlotBytes(vm, 0, buf, rd);
    free(buf);
}

void wyvern_foreign_Socket_write(WrenVM* vm) {
    wyvern_socket_info* info = (wyvern_socket_info*) wrenGetSlotForeign(vm, 0);
    if (info->fd == -1) {
        wyvern_socket_handle_error(vm, "Socket is not connected!");
        return;
    }

    int size;
    const char* buf = wrenGetSlotBytes(vm, 1, &size);
    size_t written = write(info->fd, buf, size);

    wrenSetSlotDouble(vm, 0, written);
}

void wyvern_foreign_Socket_close(WrenVM* vm) {
    wyvern_socket_info* info = (wyvern_socket_info*) wrenGetSlotForeign(vm, 0);
    if (info->fd == -1)
        return;

    close(info->fd);
    info->fd = -1;
}
