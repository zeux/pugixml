#!/usr/bin/perl

sub execprint
{
	my $cmd = shift;

	open PIPE, "$cmd |" || die "$cmd failed: $!\n";
	print while (<PIPE>);
	close PIPE;

	return $?;
}

use IO::Socket;

$exitcmd = shift;

my $client = new IO::Socket::INET(PeerAddr => "10.0.2.2:7183", Timeout => 5);
exit unless $client;

select $client;

&execprint('svn up') == 0 || die "error updating from repo\n";
&execprint('perl tests/autotest-local.pl') == 0 || die "error launching tests\n";
system($exitcmd);
