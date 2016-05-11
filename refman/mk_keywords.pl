#!/usr/bin/env perl

use strict;
use warnings;
use IO::File;

my $cmdname = $0; $cmdname =~ s{.*/}{};
my $infile = "../astl/keywords.cpp";
my $in = new IO::File $infile or die "$cmdname: unable to open $infile: $!\n";
my @keywords = ();
while (<$in>) {
   next unless (my($keyword) = m{^\s+\{"(.*?)",\sparser::token::});
   push(@keywords, $keyword);
}
@keywords = sort @keywords;
my $columns = 5;
my $count = scalar @keywords;
my $rows = $count / $columns;
++$rows if $count % $columns;

print "\\begin{tabular}{l", " l" x ($columns - 1), "}\n";
foreach my $row (0..$rows-1) {
   foreach my $column (0..$columns-1) {
      my $index = $column * $rows + $row;
      last if $index >= $count;
      print " & " if $column;
      print "\\keyword{", $keywords[$index], "}";
   }
   print " \\\\\n";
}
print "\\end{tabular}\n";
