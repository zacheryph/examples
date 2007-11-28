#!/usr/bin/env perl
use strict;

sub five_loop {
  my $meth = shift;
  printf "ref: %s\n", ref($meth);
  for (1..5) {
    &$meth($_);
  }
}

my $loopmeth = sub {
  my $num = shift;
  printf "method call: %d\n", $num;
};

five_loop $loopmeth;

printf "time: %d\n", time;

print "see\n" if 0;
