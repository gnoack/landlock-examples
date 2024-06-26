.PHONY: clean test

CFLAGS += -Wall

ifneq ($(KBUILD_OUTPUT),)
	CFLAGS += -I $(KBUILD_OUTPUT)/usr/include
endif

all: tr convert nonet tcpserver

tr: tr.o sandbox_file.o

nonet: nonet.o sandbox_socket.o landlock_compat.o

tcpserver: tcpserver.o landlock_compat.o naughty.o

convert: convert.o sandbox_file.o

test: tr
	@./test.sh

clean:
	@rm -f *.o tr nonet convert tcpserver *~

