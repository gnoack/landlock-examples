.PHONY: clean

CFLAGS = -Wall

tr: tr.o sandbox.o

clean:
	@rm -f *.o tr *~

