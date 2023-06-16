######################################################################
# Configuration file for the Shell Lab autograders
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
######################################################################

# What is the name of this Lab?
$LABNAME = "shlab";

# What are the tracefiles in SRCDIR that we should use for testing
@TRACEFILES = ("trace01.txt", "trace02.txt", "trace03.txt", 
	       "trace04.txt", "trace05.txt", "trace06.txt", 
	       "trace07.txt", "trace08.txt", "trace09.txt", 
	       "trace10.txt", "trace11.txt", "trace12.txt", 
	       "trace13.txt", "trace14.txt", "trace15.txt",
	       "trace16.txt");

# How many correctness points per trace file?
$POINTS = 5;            

# What is the max number of style points?
$MAXSTYLE = 10;

# Where are the source files and tracefiles for the driver? 
# Override with -s (grade-shlab.pl and grade-handins.pl)
$SRCDIR = "../src";

# Where is the handin directory? 
# Override with -d (grade-handins.pl)
$HANDINDIR = "./handin";



