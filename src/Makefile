#
# Makefile for codecook
#
# $Id: Makefile,v 1.19 2000/11/05 21:20:55 code Exp $
#

CC=g++
YACC=bison
LEX=flex

CFLAGS=-g -pg -Wall -D_REENTRANT -fexceptions -Woverloaded-virtual -Winline 
OBJ=thread.o ptable.o manager.o meaner.o terminal.o \
	 netmanager.o alarmserv.o codecook.o device.o device_list.o \
	 blocking.o command.o protocol.o datastruct.o \
	 cstable.o logger.o al_log.o\
	 itc.o user.o cdtree.o database.o berkeley_db.o sql.o cddb.o\
	grammar_cdtree.tab.o scanner_cdtree.o parser_cdtree.o \
	grammar_mathforms.tab.o scanner_mathforms.o parser_mathforms.o xmlgen.o

codecook: $(OBJ) ;

# C/C++ routines
%.o : %.c
	g++ -c $< $(CFLAGS)
%.o : %.cc
	g++ -c $< $(CFLAGS)

# Flex routine
%.c : %.l
	$(LEX) -o$@ $<


# Bison routines
grammar_cdtree.tab.h:grammar_cdtree.y
	bison -d -v grammar_cdtree.y -p cdtree_
grammar_cdtree.tab.c:grammar_cdtree.y
	bison grammar_cdtree.y -p cdtree_

grammar_mathforms.tab.h:grammar_mathforms.y
	bison -d -v grammar_mathforms.y -p mathforms_
grammar_mathforms.tab.c:grammar_mathforms.y
	bison grammar_mathforms.y -p mathforms_

scanner_cdtree.c scanner_mathforms.c \
grammar_cdtree.tab.c grammar_mathforms.tab.c: parsing.h
parsing.h:grammar_cdtree.tab.h grammar_mathforms.tab.h

clean:
	rm -f *.o core lex.yy.c *.tab.h *.tab.c *.output lang *~ deps\
	scanner_cdtree.c scanner_mathforms.c

dep:deps
deps:
	$(CC) -MM -MG *.cc > deps

include deps
