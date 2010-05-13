#!/bin/sh
# put this to /etc/init.d/pugixml-autotest.sh, then launch
# sudo update-rc.d pugixml-autotest.sh defaults 80
# don't forget to chmod +x pugixml-autotest.sh and to replace /home/USERNAME with actual path

if [ "$1" = "start" ]
then
	PATH=/sbin:/usr/sbin:/bin:/usr/bin:/usr/local/bin
	cd /home/USERNAME/pugixml
	perl tests/autotest-remote-host.pl "shutdown -P now" &
fi
