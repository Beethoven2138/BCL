GCCPARAMS = -Iinclude -lncurses -O3

OBJECTS=obj/main.o \
	obj/fileops.o \
	obj/buffer.o \
	obj/tui.o \
	obj/command.o \


obj/%.o: src/%.c
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

all: bcl

bcl: $(OBJECTS)
	gcc $(OBJECTS) -o bcl $(GCCPARAMS)

clean:
	rm -rf bcl obj
