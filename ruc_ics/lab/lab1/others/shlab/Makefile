# Instructor: Change DEST to the public directory where you want 
# the handout files copied to, and where the students will access them 
# from. Or you can set it when you invoke make: "make dist DEST=/foo/bar"
DEST = 

LABNAME = shlab

all: tsh-code lab-writeup handout handout-tarfile

tsh-code:
	################################
	# Build the shell solution code
	################################
	(cd src; make)

handout:
	###################################################
	# Copy the necessary files to the handout directory
	###################################################
	(rm -rf shlab-handout; mkdir shlab-handout)
	cp src/Makefile-handout shlab-handout/Makefile
	cp src/README-handout shlab-handout/README
	cp src/tsh-handout.c shlab-handout/tsh.c
	cp src/tsh shlab-handout/tshref
	cp src/sdriver.pl shlab-handout
	cp src/myspin.c shlab-handout
	cp src/mysplit.c shlab-handout
	cp src/mystop.c shlab-handout
	cp src/myint.c shlab-handout
	cp src/trace*.txt shlab-handout
	cp src/tshref.out shlab-handout

handout-tarfile: 
	########################################
	# Build tarfile of the handout directory
	########################################
	(cd shlab-handout; tar cvf ../$(LABNAME)-handout.tar *)

lab-writeup:
	########################
	# Format the lab writeup
	########################
	(cd writeup; make)

dist:
	########################################	
	# Update distribution
	########################################
	cp -p $(LABNAME)-handout.tar $(DEST)
	cp -p writeup/shlab.ps $(DEST)/$(LABNAME).ps
	cp -p writeup/shlab.pdf $(DEST)/$(LABNAME).pdf

clean:
	#################################
	# Clean the entire directory tree
	#################################
	rm -f *~ *.tar
	(rm -rf shlab-handout)
	(cd src; make clean)
	(cd writeup; make clean)
	(cd grade; make clean)

