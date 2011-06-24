
CFLAGS = -x objective-c -arch i386 -fmessage-length=0 -std=c99 \
		 -mmacosx-version-min=10.5 \
		 -fpascal-strings -fasm-blocks \
		 -framework Foundation \
		 -framework Carbon \
		 -Wall
.PHONY: clean

spaces-util: main.c Makefile
	gcc $(CFLAGS) -o spaces-util $< 
package: spaces-util
	./make_dmg.sh
clean:
	-rm spacefinder spaces-util
