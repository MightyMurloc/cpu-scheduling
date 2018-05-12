CC = gcc
CFLAGS = -I.

.PHONY: all clean

%.o: %.c
	@($(CC) -c -o $@ $< $(CFLAGS))

default: all

fcfs: sched_fcfs.o
	@gcc -o fcfs sched_fcfs.o -I.
	@rm -f sched_fcfs.o
	@mv fcfs bin/
	@echo "Made FCFS."

sjf: sched_sjf.o
	@gcc -o sjf sched_sjf.o -I.
	@rm -f sched_sjf.o
	@mv sjf bin/
	@echo "Made SJF."

srtf: sched_srtf.o
	@gcc -o srtf sched_srtf.o -I.
	@rm -f sched_srtf.o
	@mv srtf bin/
	@echo "Made SRTF."

rr: sched_rr.o
	@gcc -o rr sched_rr.o -I.
	@rm -f sched_rr.o
	@mv rr bin/
	@echo "Made RR."

all: fcfs sjf srtf rr

clean:
	@echo "Cleaning up..."
	@rm -f bin/fcfs bin/sjf bin/srtf bin/rr
	@echo "Done."
