
CFLAGS+=-std=c11
CFLAGS+=-Wall -Wextra -Wpedantic
CFLAGS+=-Wwrite-strings -Wstack-usage=1024 -Wfloat-equal -Waggregate-return -Winline


intersect: intersect.o intersect_funcs.o

profile:CFLAGS+=-pg
profile:LDFLAGS+=-pg
profile:intersect

.PHONY: clean debug profile

clean:
	rm intersect *.o

debug: CFLAGS+=-g
debug: intersect
