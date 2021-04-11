
CC = gcc
OBJS = eurovision.o euro_utility.o judge.o main.o \
state.o map.o libmtm.a
EXEC = Eurovision
DEBUG_FLAG = -DNDEBUG
COMP_FLAG = -std=c99 -Wall -Werror -pedantic-errors
LIB_FLAG= -L. -lmtm

$(EXEC) : $(OBJS)
	$(CC) $(DEBUG_FLAG) $(OBJS) $(LIB_FLAG) -o $@
eurovision.o: eurovision.c eurovision.h list.h map.h \
state.h euro_utility.h judge.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
euro_utility.o: euro_utility.c euro_utility.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
judge.o: judge.c judge.h euro_utility.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
main.o: main.c eurovision.h list.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
state.o: state.c state.h map.h euro_utility.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
map.o: map.c map.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
clean:
	rm -f $(OBJS) $(EXEC)
