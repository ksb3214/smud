#!/usr/bin/perl

open(F,"<names.dat");
$c=0;
while(not(eof(F)))
{
	$f=<F>;
	chop($f);
	if(($f)=~/(.*),(.*),(.*),(.*)/)
	{
		$f1[$c]=$1;
		$f2[$c]=$2;
		$s1[$c]=$3;
		$s2[$c]=$4;
		$c++;
	}
}
close(F);
$d=shift;
if($d eq "")
{
	$d=1;
}
while($d>0)
{
	print "name:" . $f1[rand($c)] . $f2[rand($c)] . " " . $s1[rand($c)] . $s2[rand($c)] . "\n";
	$d--;
}
