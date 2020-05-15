
include ../../common.mk

monitor:
	@gcc -c logger.c -o logger.o
	@gcc -c monitor.c -o mon.o
	@gcc logger.o mon.o -o monitor

cleanMonitor:
	@rm *.o
	@rm monitor