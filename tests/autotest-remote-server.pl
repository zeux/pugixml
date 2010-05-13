#!/usr/bin/perl

use IO::Socket;

my $server = new IO::Socket::INET(LocalPort => 7183, Listen => 1);
die "Could not create socket: $!\n" unless $server;

my $client = $server->accept();

print while (<$client>);

