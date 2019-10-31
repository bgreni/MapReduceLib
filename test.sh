#!/bin/bash

make clean-result

echo "Testing"
FILES=""
# base sample
for i in {1..1}
do
for file in sample{1..20}.txt
# for file in Book-1/06-AoS-Content-{1..10}.txt
do
    FILES+=" $file"
    # FILES+=" myTest.txt"
done
done
make
# valgrind --tool=memcheck --leak-check=yes ./wordcount 10 10 ${FILES}
# valgrind --tool=helgrind ./wordcount 10 10 ${FILES}
./wordcount 10 10 ${FILES}
# sudo gdb ./wordcount 10 10 ${FILES}
