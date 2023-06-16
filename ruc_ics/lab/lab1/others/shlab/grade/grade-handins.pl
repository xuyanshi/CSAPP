#!/usr/bin/perl 
#!/usr/local/bin/perl 
use Getopt::Std;
use lib ".";
use config;

############################################################################
# grade-handins.pl - Autograding driver program
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
# This program automatically runs an autograder on each <file> in a 
# <handin> directory, placing the resulting grade reports in the file 
# called <handin>.grades/<file>.out.
#
#  unix> ./grade-handins
# 
# Bugs:
# - Because the Unix system() call masks SIGINT signals, you can't 
#   always kill this program by typing ctrl-c. You have to
#   use kill -9 from another process to kill it.
#     
#########################################################################

# Autoflush output on every print statement
$| = 1; 

#
# usage - Print the help message and terminate
#
sub usage {
    printf STDERR "Usage: $0 [-he] [-d <directory>] [-s <srcdir>]\n";
    printf STDERR "Options:\n";
    printf STDERR "  -h             Print this message\n";
    printf STDERR "  -e             Don't include original handin file on the grade sheet\n";
    printf STDERR "  -d <handindir> Handin directory\n";
    printf STDERR "  -s <srcdir>    Directory containing autograder test codes\n";
    die "\n";
}

##############
# Main routine
##############

$0 =~ s#.*/##s;                        # this prog's basename

#
# Parse the command line arguments
#
getopts('hed:s:');
if ($opt_h) {
    usage();
}

# Override defaults in config.pm
if ($opt_s) {
    $SRCDIR = $opt_s;
}
if ($opt_d) {
    $HANDINDIR = $opt_d;
}

# 
# Set the relative paths of the autograder and the output directory
# where the grade sheets should go.
#
$grader = "grade-$LABNAME.pl";      # autograder
$output_dir = "$HANDINDIR.grades";  # directory where output grade sheets go

# 
# Make sure the command line arguments are OK
#
-e $HANDINDIR
    or die "$0: ERROR: $HANDINDIR not found\n";
-d $HANDINDIR
    or die "$0: ERROR: $HANDINDIR is not a directory\n";
-e $grader 
    or die "$0: ERROR: $grader not found\n";
-x $grader 
    or die "$0: ERROR: $grader is not an executable program\n";

#
# Get the absolute path name of the grader and the output directory
#
$cwd = `pwd`;
chomp($cwd);
$grader_abs = "$cwd/$grader";
$output_dir_abs = "$cwd/$output_dir";

#
# Create the output directory.
#
system("rm -rf $output_dir_abs");
system("mkdir $output_dir_abs") == 0
    or die "$0: ERROR: Couldn't create the $output_dir directory\n";

#
# Get a sorted list of the files (except . and ..) in the handin directory
#
opendir(DIR, $HANDINDIR) 
    or die print "Couldn't open $HANDINDIR\n";
@files = grep(!/^\.\.?$/, readdir(DIR)); # elide . and .. from the list
@files = sort(@files);
closedir(DIR);

# By default we emit the student's src code on the grade sheet
# Override this default with the -e flag
$emitarg = "";
if ($opt_e) {
    $emitarg = "-e";
}

#
# Grade each file in the handin directory.  
#
foreach $filename (@files) {
    print "Grading $HANDINDIR/$filename\n";
    system("$grader_abs $emitarg -f $HANDINDIR/$filename -s $SRCDIR > $output_dir_abs/$filename.out 2>&1") == 0 
	or print "$0: ERROR: Encountered some problem with $HANDINDIR/$filename\nSee $output_dir/$filename.out for details.\n";
}

print "$0: Finished\n";
exit;
