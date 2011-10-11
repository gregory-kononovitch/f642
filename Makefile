SHELL = /bin/sh

all:
	make -C f640
	mv f640/f640.xo .
	make -C c642
	mv c642/c642-pictu.ko .


clean:
	make -C f640 clean
	make -C c642 clean
	rm -f core* *.o f640.xo f640.1.gz c642-pictu.ko
