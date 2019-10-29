#!/bin/bash

make clean-result

echo "Testing"
FILES=""
# base sample
for i in {1..1}
do
for file in sample{1..20}.txt
# for file in Book-1/06-AoS-Content-{3..5}.txt
do
    FILES+=" $file"
    # FILES+=" myTest.txt"
done
done
make

./wordcount 10 10 ${FILES}
