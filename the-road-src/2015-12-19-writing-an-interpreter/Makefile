CC=gcc
CFLAGS=-I$(IDIR)

IDIR=include
ODIR=obj
SDIR=src
LDIR=lib

LIBS=-lreadline -lsexp

_DEPS = ml_eval.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = ml_prompt.o ml_eval.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -g -c -o $@ $< $(CFLAGS)

mlisp: $(OBJ)
	gcc -g -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm obj/*.o mlisp
