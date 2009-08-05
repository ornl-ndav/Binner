#!/bin/csh -f

set i=0
set unit=2
set step=$unit

while ($i < 4)
	@ step = $unit * $i
	../bin/tran3d $step 0 0 < ../tests/t1 > c{$i}
	@ i = $i + 1
end

cat c0 c1 c2 c3  > r0

@ i = 1

while ($i < 4)
	@ step = $unit * $i
	../bin/tran3d 0 $step 0 < r0 > r{$i}
	@ i = $i + 1
end

cat r0 r1 r2 r3 > p0

@ i = 1

while ($i < 4)
	@ step = $unit * $i
	../bin/tran3d 0 0 $step < p0 > p{$i}
	@ i = $i + 1
end

cat p0 p1 p2 p3 > v0
