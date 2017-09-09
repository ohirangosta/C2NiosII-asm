CC=gcc
CFLAGS=-I.	-O
CLIB=-ll

TARGET=parser_C
OBJ=$(TARGET).o check_C.o lexer_C.o print_NiosII.o

.SUFFIXES: .c .o

.PHONY: all

all:$(TARGET)

$(TARGET):$(OBJ) 
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(CLIB)

.c.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean-this


clean: clean-this

clean-this:
	/bin/rm	-f $(TARGET) $(OBJ) *~
