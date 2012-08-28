all:
	rm -rf f650-test.xo f650_test.o f650_graphics.o asm/graphics.ao
	yasm -f elf64 -a x86 -m amd64 -o asm/graphics.ao asm/graphics.asm
	gcc -c f650_graphics.c
	gcc -c f650_test.c
	gcc -o f650-test.xo f650_test.o f650_graphics.o asm/graphics.ao
