
include ../../common.mk

advanced-logger:
	@gcc -c testLogger.c -o test.o
	@gcc -c logger.c -o log.o
	@gcc test.o log.o -o main.o
	@./main.o