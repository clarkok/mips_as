#!/bin/sh

./mas -l label.txt $1
cat label.txt
./mas $1
./bin2coe.sh a.out > $2
