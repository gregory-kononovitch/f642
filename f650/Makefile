all:
	rm -rf f650-test.xo f650_test.o f650_graphics.o asm/graphics.ao
	yasm -f elf64 -a x86 -m amd64 -o asm/graphics.ao asm/graphics.asm
	yasm -g dwarf2 -f elf64 -a x86 -m amd64 -o asm/graphics2.ao asm/graphics2.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/graphics3.ao asm/graphics3.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/geo1.ao asm/geo1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/math1.ao asm/math1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/trig1.ao asm/trig1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/poly1.ao asm/poly1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/math0.ao asm/math0.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/std1.ao asm/std1.asm
	yasm -f elf64 -a x86 -m amd64 -o asm/brodge.ao asm/brodge.asm
	gcc -c f650_graphics.c
	gcc -c f650_geometry1.c
	gcc -c f650_test.c
	gcc -c f650_monospaced.c
	gcc -o f650-test.xo f650_test.o f650_graphics.o f650_geometry1.o 	\
	f650_monospaced.o													\
	asm/math1.ao asm/trig1.ao asm/geo1.ao 								\
	asm/graphics.ao asm/graphics2.ao asm/graphics3.ao 					\
	asm/rdtsc64.ao 														\
	asm/poly1.ao 														\
	asm/math0.ao asm/std1.ao 											\
	-lm
#	
	gcc -c f650_brodge.c
	gcc -o f650-brodge.xo f650_brodge.o f650_graphics.o f650_geometry1.o 	\
	f650_monospaced.o													\
	asm/math1.ao asm/trig1.ao asm/geo1.ao 								\
	asm/graphics.ao asm/graphics2.ao asm/graphics3.ao 					\
	asm/rdtsc64.ao 														\
	asm/poly1.ao 														\
	asm/math0.ao asm/std1.ao 											\
	asm/brodge.ao														\
	-lm
	