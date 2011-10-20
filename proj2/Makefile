
CC     = gcc
CFLAGS = -Wall -Werror -lpthread -I. -g
EXE    = ss awget

HEAD   = awget.h

all: $(EXE)
$(EXE): %: %.c $(HEAD)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXE)
