.PHONY: clean

spacefinder: main.c
	gcc -o spacefinder $< -framework Carbon
clean:
	-rm spacefinder