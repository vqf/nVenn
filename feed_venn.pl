#!/usr/bin/perl -w
use strict;

my $total = {};
my $names = [];
my $elements = [];
while (@ARGV){
  my $file = shift @ARGV;
  if (-e $file){
    open (IN, $file);
    my @l = <IN>;
    close IN;
    my $h = shift @l;
    chomp($h);
    push @$names, $h;
    my $tmp = {};
    foreach my $g (@l){
      chomp($g);
      next unless ($g =~ /[\w\d]/);
      $tmp->{$g} = 1;
      $total->{$g} = 1;
    }
    push @$elements, $tmp;
  }
  else{
    warn("Cannot find $file\n");
    
  }
}

my $ngroups = scalar(@$elements);
my $nels = 1 << $ngroups;

my $result = [];
my $numbers = [];

print "=== Group analysis ===\n";

for (1 .. $nels - 1){
  my $d = toBin($_, $ngroups);
  my $group_elements = {};
  foreach my $g (keys %$total){
    my $addme = 1;
    my $c = 0;
    foreach my $b (@$d){
      my $isingroup = exists($elements->[$c]{$g});
      my $correct = !($isingroup ^ $b);
      $addme = 0 unless ($correct);
      $c++;
    }
    if ($addme){
      $group_elements->{$g} = 1;
    }
  }
  push @$result, $group_elements;
  push @$numbers, scalar(keys %$group_elements);
  print join('-', @$d); print "\t"; print scalar(keys %$group_elements) . "\n";
  print join(', ', keys %$group_elements); print "\n";
}

print "=== nVenn input ===\n";
print "nVenn v1.2\n";
print "$ngroups\n";
print join("\n", @$names) . "\n";
print "0\n";
print join("\n", @$numbers) . "\n";


sub toBin{
  my $n = shift;
  my $ngroups = shift;
  my $result = [];
  for (1 .. $ngroups){
    my $t = $n & 1;
    unshift @$result, $t;
    $n = $n >> 1;
  }
  return $result;
}



__DATA__
Cancer	E-cadherin	GZMA	ITGA1	MYCN	MYO5A	MYO5B	NF2	P2RY8	PML	PPTN11	PRDM1	PRF1	RAS	SET	SMAD4	USP12		
Ageing	ALDH2	ATM	DCLRE1	EEF1A1	GAPDH	GSK3A	IFG2R	IGF1R 	ITGA1	MIF	NEIL1	NLN	RMI2	TERF2	XRCC5	XRCC6		
Immunity	APOBEC1	APOBEC3	CASP12	CATH	CHIA	CMA1	CTSG	gallinacinas	GZMB	GZMG	GZMH	IFN	IFTM10	IFTM5	MEP1A	NLRP	PRF1	TLRs
