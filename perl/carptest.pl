#!/usr/bin/env perl
use strict;
use Carp qw(cluck confess);

sub stack {
  my ($mod, $fn, $line, $sub);
  ($mod, $fn, $line) = caller;
  print "$sub called from $fn:$line\n";
  for (1..5) {
    ($mod, $fn, $line, $sub) = caller($_);
    print "$_: $sub called from $fn:$line\n";
  }
  print "\n";
  my $stack = Carp::longmess("We did something wrong");
  my @bt = split(/\n/, $stack);
  $stack = shift(@bt);
  $stack =~ s/^[ \t]*//;
  print "time/stack: $stack\n";
  for (@bt) {
    s/^[ \t]*/  /;
    print "time/stack: $_\n";
  }
}

sub cone {
  stack;
}

sub ctwo {
  cone(12, shift);
}

sub cthree {
  ctwo("see");
}

cthree;
