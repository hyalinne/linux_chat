.SUFFIXES	: .c .o

CC		= gcc
COMINC	= -I../src/Common/inc
COMLIB	= -L../lib -lComLib

DEFS	= -DDEBUG

CFLAGS	= -g $(DEFS) $(COMINC)
LFLAGS	= $(COMLIB) -static

EXEC	= fileLock

all		: $(EXEC)

$(EXEC)	: $(EXEC:=.o)
	$(CC) -o $@ $@.o $(LFLAGS)

clean	:
	rm -rf $(EXEC) $(EXEC:=.o) core

