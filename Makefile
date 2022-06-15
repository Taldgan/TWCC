##
# C Compiler
#

SRCDIR := src
OBJDIR := obj

OBJECTS := $(OBJDIR)/lex.o $(OBJDIR)/comp.o $(OBJDIR)/parse.o

all: comp

comp: $(OBJECTS)
	gcc $(OBJDIR)/*.o -m32 -o compiler

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	gcc -c -m32 $<
	mv *.o $(OBJDIR)

.PHONY: clean

clean:
	rm -f ./obj/*
	rm compiler

# end
