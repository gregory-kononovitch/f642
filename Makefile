SHELL = /bin/sh

p644:
	make -C p644
	mv p644/p644-brodger.xo .


all:
#	make -C f650
#	mv f650/*.xo .
#	make -C f643
#	mv f643/*.xo .
#	mv f643/*.so .
#	make -C f642
#	mv f642/*.xo .
#	mv f642/*.so .
#	make -C f640
#	mv f640/f640.xo .
	make -C f641
	mv f641/f641.xo .
#	make -C c642
#	mv c642/c642-brodge.ko .
#	make -C p643
#	mv p643/p643-brdg.xo .
#	make -C p644
#	mv p644/p644-brodger.xo .


clean:
	make -C f640 clean
	make -C f641 clean
	make -C c642 clean
	make -C p643 clean
	make -C p644 clean
	rm -f core* *.o f640.xo f641.xo f640.1.gz c642-pictu.ko p643-brdg.xo p644-brodger.xo
