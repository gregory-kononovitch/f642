SHELL = /bin/sh

all:
	make -C f640
	mv f640/f640.xo .
	make -C c642
	mv c642/c642-brodge.ko .
	make -C p643
	mv p643/p643-brdg.xo .
	make -C p644



clean:
	make -C f640 clean
	make -C c642 clean
	make -C p643 clean
	rm -f core* *.o f640.xo f640.1.gz c642-pictu.ko p643-brdg.xo
