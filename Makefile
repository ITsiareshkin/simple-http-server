# IPK Projekt 1
# Makefile
# Author: Ivan Tsiareshkin (xtsiar00)

CFLAGS = -w -Wall -Wextra -Werror -pedantic
CC = gcc
FILE = hinfosvc.c
COMPILE = -o
OUTPUT_FILE = hinfosvc
PACK_NAME = xtsiar00.zip

.PHONY: all pack clean

all:
	$(CC) $(CFLAGS) $(FILE) $(COMPILE) $(OUTPUT_FILE)

pack:
	zip $(PACK_NAME) $(FILE) Makefile README.md

clean:
	rm -rf $(OUTPUT_FILE) $(PACK_NAME)
