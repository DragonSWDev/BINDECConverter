C=gcc
CFLAGS=-std=c89 -O2 -Wall -pedantic -DUNICODE -D_UNICODE
LDFLAGS=-mwindows -lcomctl32 -s

.PHONY: clean all
all: bindec.exe
clean:
	del resources.o bindec.o bindec.exe

resources.o: resources.rc
	windres $< -o $@

bindec.exe: bindec.o resources.o
	$(C) $^ $(LDFLAGS) -o $@

bindec.o: bindec.c
	$(C) $(CFLAGS) $< -c -o $@
	