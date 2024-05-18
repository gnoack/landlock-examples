.PHONY: clean

CFLAGS = -Wall

all: tr convert

tr: tr.o sandbox.o

convert: convert.o sandbox.o

clean:
	@rm -f *.o tr convert *~

