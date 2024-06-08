.PHONY: clean

CFLAGS = -Wall

ifneq ($(KBUILD_OUTPUT),)
	CFLAGS += -I $(KBUILD_OUTPUT)/usr/include
endif

all: tr convert nonet

tr: tr.o sandbox.o

nonet: nonet.o sandbox_socket.o

convert: convert.o sandbox.o

clean:
	@rm -f *.o tr nonet convert *~

