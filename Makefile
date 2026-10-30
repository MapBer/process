CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -g
EXERCISES := $(sort $(patsubst %/main.c,%,$(wildcard */main.c)))
TARGETS := $(addprefix bin/,$(EXERCISES))
TARGETS += bin/12_tcp_chat_server bin/12_tcp_chat_client

.PHONY: all clean $(EXERCISES) 12_tcp_chat
all: $(TARGETS)

12_tcp_chat: bin/12_tcp_chat_server bin/12_tcp_chat_client

bin/12_tcp_chat_%: 12_tcp_chat/%.c | bin
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

$(EXERCISES): %: bin/%

bin/%: %/main.c | bin
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

bin:
	mkdir -p $@

clean:
	rm -f $(TARGETS)
