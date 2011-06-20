CFLAGS = -x objective-c -arch i386 -fmessage-length=0 -std=c99 \
		 -fpascal-strings -fasm-blocks \
		-isysroot /Developer/SDKs/MacOSX10.5.sdk \
		-mmacosx-version-min=10.5 -framework Foundation \
		-framework Carbon

.PHONY: clean

spacefinder: main.c
	gcc $(CFLAGS) -o spacefinder $< 
clean:
	-rm spacefinder
