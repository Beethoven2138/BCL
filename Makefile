GCCPARAMS = -O3 -Iinclude

OBJECTS=obj/main.o \
	obj/fileops.o \
	obj/textcolor.o \
	obj/buffer.o \


obj/%.o: src/%.c
	mkdir -p $(@D)
	gcc $(GCCPARAMS) -c -o $@ $<

all: text_editor

text_editor: $(OBJECTS)
	gcc $(OBJECTS) -o text_editor $(GCCPARAMS)

clean:
	rm -rf *.o text_editor obj
