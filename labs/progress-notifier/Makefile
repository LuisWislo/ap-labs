
include ../../common.mk

notifier:
	@gcc -c logger.c -o logger.o
	@gcc -c base64.c -o base.o
	@gcc logger.o base.o -o base64