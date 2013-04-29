#!/usr/bin/perl -w

use strict;
use warnings;

open CSV, "areacodes.csv" || die $!;
my $csv = <CSV>;
close CSV;
my @areacodes = split(/,/, $csv);

my $count = $ARGV[0];

for ( my $i = 0; $i < $count; $i++ ) {
    my $ac = $areacodes[int(rand($#areacodes))];
    my $num = int(rand(9_999_999));
    printf("%03d%07d\n", $ac, $num);
}
