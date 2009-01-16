#!/bin/bash

# use: bmeshrebin_server hostname port_number resolution

TOP=..
serveproc="${TOP}/bin/serve"
rebinproc="${TOP}/bin/bmeshrebin"

$serveproc $1 $2 | $rebinproc $3

exit 0


