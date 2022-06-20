##
# C Compiler
#

SRCDIR := src
OBJDIR := obj

OBJECTS := $(OBJDIR)/lex.o $(OBJDIR)/comp.o $(OBJDIR)/parse.o $(OBJDIR)/gen.o

all: comp

comp: $(OBJECTS)
	gcc $(OBJDIR)/*.o -ggdb -m32 -o compiler

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(OBJDIR)
	gcc -c -m32 -ggdb $< -o $@

$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY: clean

clean:
	rm -f $(OBJDIR)/*.o
	rm -f compiler

# end
