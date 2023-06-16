NAME = shlab

all:
	latex $(NAME).tex
	latex $(NAME).tex # again to resolve references
	dvips -o $(NAME).ps $(NAME).dvi
	ps2pdf $(NAME).ps

clean:
	rm -f *.aux *.ps *.pdf *.dvi *.log *~

