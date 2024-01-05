#!/bin/bash

echo '' > task1_check.diff

total=0
passed=0
failed=0
for file in $(ls dat/task1/*)
do
	testname=$(basename -s .dat $file)
	./task1_test < $file > out/task1/$testname.out.unsorted
	sort -t ',' -k 1 -k 2 out/task1/$testname.out.unsorted > out/task1/$testname.out
	rm out/task1/*.out.unsorted
	diff -c exp/task1/$testname.out out/task1/$testname.out >> task1_check.diff
	if [ $? -eq 0 ]; then
		echo "$testname passed"
		((passed+=1))
	else
		echo "$testname failed"
		((failed+=1))
	fi
	((total+=1))
done

echo "=========================================================================="
echo "test result: $passed/$total passed, $failed failed"
