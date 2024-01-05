all: task1_test task2_test task3_test

task1_benchmark: task1_benchmark.cc task1.cc row.cc
	g++ -O0 -g3 task1.cc task1_benchmark.cc row.cc -std=c++11 -isystem -I.   \
	    -Ibenchmark/include -Lbenchmark/lib -lbenchmark -lpthread            \
        -o task1_benchmark


task2_benchmark: task2_benchmark.cc task2.cc row.cc
	g++ -O0 -g3 task2.cc task2_benchmark.cc row.cc -std=c++11 -isystem -I.   \
	    -Ibenchmark/include -Lbenchmark/lib -lbenchmark -lpthread            \
        -o task2_benchmark

task3_benchmark: task3_benchmark.cc task3.cc row.cc
	g++ -O0 -g3 task3.cc task3_benchmark.cc row.cc -std=c++11 -isystem -I.   \
	    -Ibenchmark/include -Lbenchmark/lib -lbenchmark -lpthread            \
        -o task3_benchmark

task1_test: task1.cc task1_test.cc row.cc
	g++ -O0 -g3 task1.cc task1_test.cc row.cc -std=c++11 -isystem -I.        \
	    -lpthread                                                            \
        -o task1_test

task2_test: task2.cc task2_test.cc  row.cc
	g++ -O0 -g3 task2.cc task2_test.cc row.cc -std=c++11 -isystem -I.        \
	    -lpthread                                                            \
        -o task2_test

task3_test: task3.cc task3_test.cc row.cc
	g++ -O0 -g3 task3.cc task3_test.cc row.cc -std=c++11 -isystem -I.        \
	    -lpthread                                                            \
        -o task3_test
check: task1_test task2_test task3_test
	bash task1_check.sh
	bash task2_check.sh
	bash task3_check.sh

benchmark: task1_benchmark task2_benchmark task3_benchmark
	./task1_benchmark > task1_benchmark.out
	./task2_benchmark > task2_benchmark.out
	./task3_benchmark > task3_benchmark.out
	tail -n 4 task*_benchmark.out

data:
	g++ data.cc -o data_generator
	./data_generator > testxxx.dat
	./task1_test < testxxx.dat > task1_testxxx.out.ori
	./task2_test < testxxx.dat > task2_testxxx.out.ori
	./task3_test < testxxx.dat > task3_testxxx.out.ori
	sort -t ',' -k 1 -k 2 task1_testxxx.out.ori > task1_testxxx.out.sorted
	sort -t ',' -k 1 -k 2 task2_testxxx.out.ori > task2_testxxx.out.sorted
	sort -t ',' -k 1 -k 2 task3_testxxx.out.ori > task3_testxxx.out.sorted
	diff -c task1_testxxx.out.sorted task2_testxxx.out.sorted
	diff -c task1_testxxx.out.sorted task3_testxxx.out.sorted
	diff -c task2_testxxx.out.sorted task3_testxxx.out.sorted


clean:
	rm -f *_check.diff *_test data_generator *_benchmark *_benchmark.out     \
              out/task1/*                                                    \
              out/task2/*                                                    \
              out/task3/*
