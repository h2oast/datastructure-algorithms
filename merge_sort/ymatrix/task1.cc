#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/sysinfo.h>
#include "row.h"

typedef struct Thread
{
    pthread_t         id;
    int               index;
    const Row        *rows;
    int               count;
} Thread;

/*
 * calculate a resonable degree of parallel
 *
 * here we let it be: maximum(2, 2 * ncpu)
 */
static int
parallel_degree()
{
    int ncpu = get_nprocs();

    return (ncpu <= 0) ? 2 : 2 * ncpu;
}

/*
 * Given a thread identified by its index, we arrange a scan range for it.
 *
 * The result range is a integer sequence: [start, end)
 */
static void
calculate_scan_range(int nrows, int nthread, int thread_index, int *start, int *end)
{
    int nrow_per_thread = (nrows + (nthread - 1)) / nthread;

    /*
     * Just keep it simple here, when the total number  of rows is less than
     * the number of threads, let the front nrows thread to do a one row scan,
     * and the left (nthread - nrows) thread(s) do nothing at all.
     */
    if (nrows < nthread)
    {
        *start = thread_index < nrows ? thread_index : -1;
        *end   = thread_index < nrows ? (thread_index + 1) : -1;
 
        return;
    }
    
    *start = nrow_per_thread * thread_index;
    if (thread_index < (nthread - 1))
    {
        *end = (thread_index + 1) * nrow_per_thread;
    }
    else
    {   
        /*
         * The last thread's end of its scan range
         */
        *end = nrows;
    }
}

/*
 * The condition rows should meet.
 */
static int
condition(const Row *row)
{
    if ((row->b >= 10 && row->b < 500000) &&
        (row->a == 10 || row->a == 200 || row->a == 3000))
    {
        return TRUE;
    }
    else 
    {
        return FALSE;
    }
}

/*
 * Worker thread's main work
 *
 * Pick up my work range in the rows, then scan the range to filter out the 
 * rows satisfied the condition
 */
static void*
work(void *datum)
{
    Thread *thread = (Thread *)datum;

    for (int which = 0; which < thread->count; which++)
    {
        if (condition(thread->rows + which))
        {
            row_printer(thread->rows + which);
        }
    }

    return NULL;
}

/*
 *
 */
static void
launch_worker_threads(const Row *rows, int nrow, Thread *threads, int nthread)
{
    pthread_attr_t attr;
    int            ret = 0;

    if ((ret = pthread_attr_init(&attr)) != 0)
    {
        fprintf(stderr, "error: pthread_attr_init, %d, errno: %d\n",
                ret, errno);
        exit(1);
    }

    for (int idx = 0; idx < nthread; idx++)
    {
        int start = 0;
        int end   = 0;

        calculate_scan_range(nrow, nthread, idx, &start, &end);
        threads[idx].rows = &rows[start];
        threads[idx].count = end - start;
        threads[idx].index = idx;
        if (0 != (ret = pthread_create(&threads[idx].id, &attr, work, threads + idx)))
        {
            fprintf(stderr, "error: pthread_create, %d, errno: %d\n", ret, errno);
            exit(1);
        }
    }
    
    if ((ret = pthread_attr_destroy(&attr)) != 0)
    {
        fprintf(stderr, "error: pthread_attr_destroy, %d, errno: %d\n", ret, errno);
        exit(1);
    }
}

/*
 * Wait for all thread finished
 *
 */
void
wait_worker_threads(Thread *threads, int nthread)
{
    int   ret    = 0;

    for (int idx = 0; idx < nthread; idx++)
    {
        if (ret = pthread_join(threads[idx].id, NULL) != 0)
        {
            fprintf(stderr, "error: pthread_join, %d, errno: %d\n", ret, errno);
            exit(1);
        }
    }
}

/*!
 * Task 1.
 *
 * Find out all the rows that satisfy below conditions:
 *
 *     ((b >= 10 && b < 500000) &&
 *      (a == 10 || a == 200 || a == 3000))
 *
 * Print them to the terminal, one row per line, for example:
 *
 *     10,31
 *     10,23
 *     200,16
 *
 * @param nrows The total number of rows.
 * @param rows  The rows, for example rows[0] is the first row.
 */
void
task1(const Row *rows, int nrows)
{
    Thread *threads          = NULL;
    int     nthread          = 0;

    if ((rows == NULL) || (nrows <= 0))
        return;

    nthread = parallel_degree();
    threads = (Thread *)malloc(nthread * sizeof(Thread));
    memset(threads, 0, nthread * sizeof(Thread));

    /*
     * Create threads and start them, each thread take a range, scan rows
     * of the ranges
     */
    
    launch_worker_threads(rows, nrows, threads, nthread);

    /*
     * Wait all threads finished
     */
    wait_worker_threads(threads, nthread);

    free(threads);
}