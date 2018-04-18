LDLIBS = -lm -lwren -ldl
CFLAGS = -Wall -Wno-unused-function -rdynamic

default: wyvern.c
	gcc $(LDLIBS) $(CFLAGS) $^ -o wyvern
