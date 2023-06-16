#!/usr/bin/perl 
#!/usr/local/bin/perl 
use Getopt::Std;

############################################################################
# printdir - prints files in the current directory using enscript
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
# This program uses enscript to print all the text files in 
# the current directory that match some regexp pattern.
#########################################################################

# Autoflush output on every print statement
$| = 1; 

# Enscript arguments
$STYLE = "-G -2r";       # enscript printing style (Gaudy, 2-up)
$FONTNAME  = "Courier";  # enscript fontname

#
# usage - Print the help message and terminate
#
sub usage {
    printf STDERR "$_[0]\n";
    printf STDERR "Usage: $0 [-he] -P <printer> [-s <fontsize> -p <pattern>]\n";
    printf STDERR "Options:\n";
    printf STDERR "  -P <printer>    Printer name\n";
    printf STDERR "  -s <fontsize>   Printing font size (default 8)\n";
    printf STDERR "  -p <pattern>    Filename pattern (default *)\n";
    printf STDERR "  -h              Print this message\n";
    printf STDERR "  -e              Echo the files that would be printed\n";
    die "\n";
}

##############
# Main routine
##############

$0 =~ s#.*/##s;   # this prog's basename
$fontsize = 8;    # default fontsize
$pattern = ".*";  # default pattern (all files in $dir)

#
# Parse the command line arguments
#
getopts('hep:s:P:');
if ($opt_h) {
    usage();
}
$dir = ".";
$echo = $opt_e; 
$printer = $opt_P; 

if (!$opt_P) { 
    usage("Missing required printer (-P) arg");
}

if ($opt_s) {
    $fontsize = $opt_s;
}
if ($opt_p) {
    $pattern = $opt_p;
}

#
# Get a sorted list of the files (except . and ..) in the target dir
#
opendir(DIR, $dir) 
    or die print "Couldn't open $dir\n";
@files = grep(!/^\.\.?$/, readdir(DIR)); # elide . and .. from the list
@files = grep(/$pattern/, @files);
@files = sort(@files);
closedir(DIR);

#
# Print each file in the directory that matches "pattern"
#
$command = "enscript -P$printer $STYLE -f$FONTNAME$fontsize";
foreach $filename (@files) {
    print "$command $filename\n";
    if (!$echo) {
	system("cd $dir; $command $filename 2>&1") == 0 
	    or print "$0: ERROR: couldn't print $filename\n";
    }
}
exit;
