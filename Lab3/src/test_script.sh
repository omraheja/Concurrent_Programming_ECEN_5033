#!/bin/bash
shuf -r -n 2000000 -i 0-10000000 > script_test_file.txt
sort -n script_test_file.txt > script_test_file_sorted.txt
make
./mysort script_test_file.txt -o output.txt
cmp --silent output.txt script_test_file_sorted.txt && echo "Same!" || echo "Different!"

