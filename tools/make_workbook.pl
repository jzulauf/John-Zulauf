#!/usr/bin/perl

use Spreadsheet::WriteExcel;

while( @ARGV )  {
	my $a = shift @ARGV;

	$a =~/^([a-z]+)_(\w+)_(\d{6}_\d{4})_to(\d+)(-R|-R-A)\.csv/ && do {
		my $fun = $1;
		my $desc = $2;
		my $date = $3;
		my $size = $4;
		my $type = $5;
		my $file = $desc . ".xls";
		my $sheet = $fun . " " .$size. " " .$type;

		my $data = { fun => $fun, size=> $size, type => $type, can => {} };
		$file_data->{$fun}->{$size}->{$type} = $data;
		$group{$fun} = $fun;

		open SH, $a || do {
			warn "Skipping $a, can't open.";
			next;
		};
		if ( ! $workbook ) {
			$workbook = Spreadsheet::WriteExcel->new($file)
				|| die "Can't create $name";
		}
		$worksheet = $workbook->add_worksheet($sheet);

		@lines = <SH>;
		my $row = 0;
		foreach $l (@lines) {
			my $col = 0;
			chomp $l;
			my @cells = split ",", $l;
			my $data_name = "";
			while (@cells) {
				my $c = shift @cells;
				$c =~ s/^(==.*)/'$1/;
				$data_name = $c if 0 == $col;
				$c =~ /^ratio_(\w+)/ && do {
					$data->{can}->{$1} = 1;
				};
				if ( $data_name  && 1 == $col ) {
					$data->{$data_name} = $c;
				}
				if ( $c =~ /^\s*-*(\d+|\.\d+|\d+\.\d+|\.\d+)\s*$/ ) {
					#print "$c\n";
					$worksheet->write_number($row, $col++, $c);
				} else {
					$worksheet->write($row, $col++, $c);
				}
				# look for special cells for the at a glance
				# TODO
			}
			$row++;
		}
		next;
	};
	warn "unrecoginzed filename formatting $a";
	
}

%types= ( "-R" => "aligned", "-R-A" => "unaligned" );
%sizes= ( "128" => "small", "2049" => "large" );
if ($workbook){
	my $waag = $workbook->add_worksheet("At a glance");
	my $wsum = $workbook->add_worksheet("Summary");

	my $row = 0;
	my $col = 0;
	my $s_row = 0;
	my $s_col = 0;
	foreach my $fun ( keys %group ) {
		my $sum = {};
		my $fs = $file_data->{$fun};
		foreach my $s (keys %$fs ) {
			my $ft = $fs->{$s};
			foreach my $t (keys %$ft ) {
				my $size = $s;
				$size = $sizes{$s} if exists $sizes{$s};
				my $type = $t;
				$type = $types{$t} if exists $types{$t};
				my $name = $fun . " " . $size . " " . $type;
				my $d = $ft->{$t};
				my $can = $d->{best_test};
				my $val = $d->{"ratio_".$can};
				$name = "$can:" . $name;
				$waag->write($row  ,$col, $name);
				$waag->write_number($row++,$col+1, $val);
				my $cl = $d->{can};
				foreach $can ( keys %$cl ) {
					$sum->{$can}->{val} += $d->{"ratio_".$can};
					$sum->{$can}->{count}++;
					#print "Rc:",$can,"\n";
					#print "RR:",$d->{"ratio_".$can},"\n";
					#print "RV:$sum->{$can}->{val}\n";
					#print "RC:$sum->{$can}->{count}\n";
				}
			}
		}
		my $best = "";
		my $best_val = 0;
		for my $can ( keys %$sum ) {
			#print "V:$sum->{$can}->{val}\n";
			#print "C:$sum->{$can}->{count}\n";
			my $val = $sum->{$can}->{val} / $sum->{$can}->{count};
			if ( $val > $best_val ) {
				$best = $can;
				$best_val = $val;
			}
		}
		my $name = $best . ":".$fun;
		$wsum->write($s_row  ,$s_col  ,$name);
		$wsum->write_number($s_row++,$s_col+1,$best_val);

	}

	$workbook->close();
}
