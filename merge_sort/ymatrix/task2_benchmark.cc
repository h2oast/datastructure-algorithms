#include <benchmark/benchmark.h>
#include "row.h"

#define A_MAX            3000
#define B_MAX           50000
static Row rows[A_MAX * B_MAX];


static void
prepare_rows(Row *rows, int nrow)
{
    for (int i = 0; i < A_MAX; i++)
    {
        for (int j = 0; j < B_MAX; j++)
        {
            rows[i * B_MAX + j].a = i + 1;
            rows[i * B_MAX + j].b = j + 1;
        }
    }
}

static void
do_setup(const benchmark::State& state)
{
    prepare_rows(rows, A_MAX * B_MAX);    
}

static void BM_task2(benchmark::State& state)
{
    for (auto _ : state)
        task2(rows, A_MAX * B_MAX);
}

// Register the function as a benchmark
BENCHMARK(BM_task2)->Setup(do_setup);

BENCHMARK_MAIN();