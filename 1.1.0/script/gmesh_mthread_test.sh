#!/bin/bash

time map -n 2 gmeshrebin3 -f 0.0 2.0 2.0 0.0 6.0 3.0 0.0 6.0 3.0 < ../data/reuter_input.inb > /dev/null

time map -n 3 gmeshrebin3 -f -0.8 0 0.002 -0.21 0.21 0.001 0.5 2.0 0.002 < ../exp/rotdata_test.inb > /dev/null

