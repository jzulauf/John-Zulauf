#!/usr/bin/perl

$def =	"#ifdef DO_PREX\n" .
	"#define PREX prefetch 0x20(%esi)\n" .
	"#else\n" .
	"#define PREX\n".
	"#endif\n";

for ( $i = 0 ; $i < 29 ; $i++ ) {
	$t =$def;
	$t =~ s/X/$i/g;
	print "$t\n";
}
