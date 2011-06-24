
CFLAGS = -x objective-c -arch i386 -fmessage-length=0 -std=c99 \
		 -mmacosx-version-min=10.5 \
		 -fpascal-strings -fasm-blocks \
		 -framework Foundation \
		 -framework Carbon \
		 -Wall
.PHONY: clean

spacefinder: main.c Makefile
	gcc $(CFLAGS) -o spacefinder $< 
clean:
	-rm spacefinder
