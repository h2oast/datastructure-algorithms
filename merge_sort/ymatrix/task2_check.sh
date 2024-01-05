#!/bin/bash

echo '' > task2_check.diff

total=0
passed=0
failed=0
for file in $(ls dat/task2/*)
do
	testname=$(basename -s .dat $file)
	./task2_test < $file > out/task2/$testname.out
	diff -c exp/task2/$testname.out out/task2/$testname.out >> task2_check.diff
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
