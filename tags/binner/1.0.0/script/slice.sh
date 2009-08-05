#!/bin/csh -f

# run using: slice.sh vol.bin.d which_axis firstslice lastslice
if ($#argv != 6) then 
        echo "usage: slice.sh vol.bin.d which_axis firstslice lastslice minval maxval"
        exit 1
        endif

set data=$1
set i=$2
@ j=$3
@ k=$4
set minval=$5
set maxval=$6

while ($j <= $k)
   ../bin/bslicer $data $i $j $minval $maxval
   mv bslice.bmp s{$j}.bmp
   @ j = $j + 1
end

