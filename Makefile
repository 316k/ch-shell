# Makefile pour CH.  Rien de bien compliqué ici.

# Attention, dans les fichiers Make, les TAB ont une signification très
# spécifique.  Ne pas remplacer des TABs par des espaces (ou l'inverse).

CFLAGS=-Wall -g

CC=cc
LD=cc
RM=rm -f
PDFLATEX=pdflatex

# Point d'entrée principal
all: ch rapport.pdf #tp-shell.pdf

# Comment construire "ch".
ch: ch.o
	$(LD) $(LDFLAGS) -o ch ch.o

# Règle générique pour contruire un ".o" à partir d'un ".c".
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) *.o ch *.aux *.log *.pdf

install:
	cp ./ch ~/bin/ch

.SUFFIXES: .tex .pdf

# Règle simpliste pour générer le PDF à partir du source LaTeX.
.tex.pdf:
	$(PDFLATEX) $<

