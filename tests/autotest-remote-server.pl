#!/usr/bin/perl

use IO::Socket;

$vm = shift;

my $server = new IO::Socket::INET(LocalPort => 7183, Listen => 1);
die "Could not create socket: $!\n" unless $server;

system("vboxmanage startvm $vm --type gui");

print "Listening for connection...\n";

my $client = $server->accept();

print while (<$client>);

