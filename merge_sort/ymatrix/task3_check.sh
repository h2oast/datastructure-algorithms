#!/bin/bash

echo '' > task3_check.diff

total=0
passed=0
failed=0
for file in $(ls dat/task3/*)
do
	testname=$(basename -s .dat $file)
	./task3_test < $file > out/task3/$testname.out
	diff -c exp/task3/$testname.out out/task3/$testname.out >> task3_check.diff
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
