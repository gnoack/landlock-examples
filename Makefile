.PHONY: clean

CFLAGS += -Wall

ifneq ($(KBUILD_OUTPUT),)
	CFLAGS += -I $(KBUILD_OUTPUT)/usr/include
endif

all: tr convert nonet tcpserver

tr: tr.o sandbox_file.o

nonet: nonet.o sandbox_socket.o

tcpserver: tcpserver.o sandbox_socket.o naughty.o

convert: convert.o sandbox_file.o

clean:
	@rm -f *.o tr nonet convert tcpserver *~

