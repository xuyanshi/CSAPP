
DRIVER = ./sdriver.pl
TSHARGS = "-p"
ELIDE = ./elide.pl
CC = gcc
CFLAGS = -Wall -O2

FILES = tsh tshref tsh-handout myspin mysplit mystop myint

all: $(FILES)

# The students get a partial solution to start with
tsh-handout.c: tsh.c
	$(ELIDE) -t handout < tsh.c > tsh-handout.c

tshref: tsh
	cp tsh tshref	

tshrefout:
	make tests > tshref.out 2>&1

# 
# Regression tests
#
tests: 	test01 test02 test03 test04 test05 test06 test07 test08 test09 test10\
	test11 test12 test13 test14 test15 test16
test01:
	$(DRIVER) -t trace01.txt -s ./tsh -a $(TSHARGS)
test02:
	$(DRIVER) -t trace02.txt -s ./tsh -a $(TSHARGS)
test03:
	$(DRIVER) -t trace03.txt -s ./tsh -a $(TSHARGS)
test04:
	$(DRIVER) -t trace04.txt -s ./tsh -a $(TSHARGS)
test05:
	$(DRIVER) -t trace05.txt -s ./tsh -a $(TSHARGS)
test06:
	$(DRIVER) -t trace06.txt -s ./tsh -a $(TSHARGS)
test07:
	$(DRIVER) -t trace07.txt -s ./tsh -a $(TSHARGS)
test08:
	$(DRIVER) -t trace08.txt -s ./tsh -a $(TSHARGS)
test09:
	$(DRIVER) -t trace09.txt -s ./tsh -a $(TSHARGS)
test10:
	$(DRIVER) -t trace10.txt -s ./tsh -a $(TSHARGS)
test11:
	$(DRIVER) -t trace11.txt -s ./tsh -a $(TSHARGS)
test12:
	$(DRIVER) -t trace12.txt -s ./tsh -a $(TSHARGS)
test13:
	$(DRIVER) -t trace13.txt -s ./tsh -a $(TSHARGS)
test14:
	$(DRIVER) -t trace14.txt -s ./tsh -a $(TSHARGS)
test15:
	$(DRIVER) -t trace15.txt -s ./tsh -a $(TSHARGS)
test16:
	$(DRIVER) -t trace16.txt -s ./tsh -a $(TSHARGS)


# clean up
clean:
	rm -f $(FILES) tsh-handout.c *.o *~

