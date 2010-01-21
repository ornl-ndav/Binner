#!/bin/bash

#test 1
echo -n "test 1: "
map -n 2 gmeshrebin3 -f 0.0 2.0 2.0 0.0 6.0 2.0 0.0 6.0 2.0 < ../data/reuter_input.inb 2>&1 | grep "total cnt" 

#test 2
echo -n "test 2: "
map -n 2 gmeshrebin3 -f 0.0 2.0 1.0 0.0 6.0 2.0 0.0 6.0 2.0 < ../data/reuter_input.inb 2>&1 | grep "total cnt"

#test 3
echo -n "test 3: "
map -n 2 gmeshrebin3 -f 0.0 2.0 2.0 0.0 6.0 3.0 0.0 6.0 3.0 < ../data/reuter_input.inb 2>&1 | grep "total cnt"

#test 4
echo -n "test 4: "
map -n 2 gmeshrebin3 -f 0.0 2.0 1.0 0.0 6.0 3.0 0.0 6.0 3.0 < ../data/reuter_input.inb 2>&1 | grep "total cnt"

#test 5
echo -n "test 5: "
map -n 2 gmeshrebin3 -f 0.0 2.0 2.0 0.0 6.0 3.0 0.0 6.0 6.0 < ../data/reuter_input.inb 2>&1 | grep "total cnt"

