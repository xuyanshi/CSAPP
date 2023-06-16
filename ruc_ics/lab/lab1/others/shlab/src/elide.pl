#!/usr/bin/perl 
#!/usr/local/bin/perl
use Getopt::Std;

####################################################################
# elide.pl - simple filter that elides any text surrounded by a tag
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
# Usage: elide -t <tag>
# Options:
#   -t <tag>     tag that delimits the text to be extracted (required)
#
# Scans stdin and echos any text line NOT contained between pairs of 
# directives of the form:
#   /* $begin <tag> */
#   ...echos everything here except other directives
#   /* $end <tag> */ 
#
####################################################################

# Parse the command line arguments
getopts('t:');
if (!$opt_t) {
    printf STDERR "Usage: $0 -t <tag>\n";
    die "\n";
}

# Check the command line argument
$tag = $opt_t;

# Files are read from stdin and written to stdout
$infile = STDIN;
$outfile = STDOUT;

# Initialize various state variables.
$echoline = 1; # initially, don't everything
$found = 0;    # becomes true when we find the tag
$toggle = 0;   # toggled each time we find the tag
$clinenum = 0; # current input line number

# Main loop
while (<$infile>) {
    $clinenum++;
    $line = $_;

    # Matches a $begin directive
    if ($line =~ /\/\* \$begin $tag \*\// ) { 
	if ($toggle) {
	    system("rm -f $tmpfilename"); 
	    die "$0: ERROR (line $clinenum): Bad nesting. Found \$begin, expected \$end.\n";
	}
	$echoline = 0;
	$found = 1;
	$toggle = ($toggle + 1) % 2;
    }

    # Matches a $end directive
    elsif ($line =~ /\/\* \$end $tag \*\//) { 
	if (!$toggle) {
	    system("rm -f $tmpfilename"); 
	    die "$0: ERROR (line $clinenum): Bad nesting. Found \$end, expected \$begin.\n";
	}
	$echoline = 1;
	$toggle = ($toggle + 1) % 2;
    }

    # Elides nested nonmatching tags
    elsif (($line =~ /\/\* \$begin/) ||
	   ($line =~ /\/\* \$end/)) {
    } 

    # echos a line
    elsif ($echoline) {                
	printf $outfile "%s", $line;
    }
}

# clean up and do some error checking 
if (!$found) {
    die "$0: ERROR: Couldn't find tag $tag in $infile\n";
}
if ($toggle) {
    die "$0: ERROR: Nonmatching begin/end directives in $infile\n"; 
}

exit;
