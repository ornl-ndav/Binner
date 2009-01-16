#!/bin/bash

# use: bmeshrebin_server hostname port_number resolution

TOP=..
serveproc="${TOP}/bin/serve"
rebinproc="${TOP}/bin/bmeshrebin"
logfile="${TOP}/log/bmeshrebin_server.log"
$serveproc $1 $2 2>>$logfile | $rebinproc $3 2>>$logfile

echo >> $logfile
exit 0


