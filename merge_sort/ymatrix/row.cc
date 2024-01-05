#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/sysinfo.h>
#include "row.h"

typedef struct Thread
{
    pthread_t         id;          /* thread in the system                       */
    int               index;       /* thread index in the thread array           */

    /*
     * shared among all threads
     */
    const Row        *rows;        /* the whole array containing all rows        */
    int               nrow;        /* the array size                             */

    /*
     * thread specific data, here it is the range for which this thread should 
     * search the lower or upper boundary.
     */
    Range            *range;
} Thread;

/*
 * Print the row to terminal
 */
void
row_printer(const Row *row)
{
    printf("%d,%d\n", row->a, row->b);
}

static void *
determine_range_boundary(void *datum);

/*
 * calculate a resonable degree of parallel
 *
 * here we let it be: 2 * nrange, each range has two threads, one is searching
 * for its lower boundary and another one is searching for its upper boundary.
 * 
 */
static int
parallel_degree(int nrange)
{
    return 2 * nrange;
}

/*
 *=======================================================================================
 */
static void
launch_worker_threads(Thread *threads, int nthread, void *(*job)(void *))
{
    pthread_attr_t attr;
    int            ret = 0;

    if ((ret = pthread_attr_init(&attr)) != 0)
    {
        fprintf(stderr, "error: pthread_attr_init, %d, errno: %d\n", ret, errno);
        exit(1);
    }

    for (int idx = 0; idx < nthread; idx++)
    {
        if (0 != (ret = pthread_create(&threads[idx].id, &attr, job, threads + idx)))
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
static void
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

/*
 *=======================================================================================
 */
 
/*
 * find the left most row that is great or equal the given range's lower row. 
 *
 * a variant of binary search method.
 */
static void
determine_range_lower(const Row *rows, int nrow, Range *range)
{
    int  left  = 0;
    int  right = nrow;
    Row *lower = &range->lower;

    /*
     * empty range due to its upper row value is less than the head row of
     * the whole rows, as the following illustrated case:
     *
     *     rows :              [.............................]
     *     range:    [.........)
     *
     * empty range due to its lower row value is great than the tail row of
     * the whole rows, as the following illustrated case:
     *
     *     rows :              [.............................]
     *     range:                                             [.........)
     *
     * Case range is out of rows, assumption is range->rows is initialized
     * to be NULL already.
     */
    if (row_compare_le(&range->upper, &rows[0]) ||
        row_compare_gt(&range->lower, &rows[nrow - 1]))
    {
        return;
    }
    
    range->rows = rows;

    /*
     * optimize corner case:
     *     rows :              [.............................]
     *     range:       [.........)
     * 
     * first row of rows as the lower boundary of our seaching result
     */
    if (row_compare_ge(&rows[0], lower))
    {
        range->lower_index = 0;

        return;
    }

    for (; (left + 1) < right;)
    {
        int middle = (left + right)  / 2;

        if (row_compare_ge(&rows[middle], lower))
        {
            right = middle;
        }
        else 
        {
            left  = middle;
        }
    }
    range->lower_index = right;
}


/*
 * find the row that behand the right most row that is great or equal the 
 * given range's upper row. 
 *
 * a variant of binary search method.
 */
static void
determine_range_upper(const Row *rows, int nrow, Range *range)
{
    int  left  = 0;
    int  right = nrow;
    Row *upper = &range->upper;

    /*
     * empty range due to its upper row value is less than the head row of
     * the whole rows, as the following illustrated case:
     *
     *     rows :              [.............................]
     *     range:    [.........)
     *
     * empty range due to its lower row value is great than the tail row of
     * the whole rows, as the following illustrated case:
     *
     *     rows :              [.............................]
     *     range:                                             [.........)
     *
     * Case range is out of rows, assumption is range->rows is initialized
     * to be NULL already.
     */
    if (row_compare_le(&range->upper, &rows[0]) ||
        row_compare_gt(&range->lower, &rows[nrow - 1]))
    {
        return;
    }

    range->rows = rows;

    /*
     * optimize corner case:
     *     rows :              [.............................]
     *     range:                                       [.........)
     * 
     * last row of rows as the upper boundary of our seaching result
     */
    if (row_compare_lt(&rows[right - 1], upper))
    {
        range->upper_index = right;
    }

    for (; (left + 1) < right;)
    {
        int middle = (left + right)  / 2;

        if (row_compare_ge(&rows[middle], upper))
        {
            right = middle;
        }
        else 
        {
            left  = middle;
        }
    }
    range->upper_index = right;
}


/*
 * thread 
 *     > with even index search for the lower boundary for the range,
 *     > with odd index search for the upper boundary for the range.
 */
static void *
determine_range_boundary(void *datum)
{
    Thread *thread = (Thread *)datum;

    if ((thread->index % 2) == 0)
    {
        determine_range_lower(thread->rows, thread->nrow, thread->range);
    }
    else
    {
        determine_range_upper(thread->rows, thread->nrow, thread->range);
    }

    return NULL;
}

static Thread *
initialize_threads(const Row *rows, int nrow, Range *ranges, int nrange)
{
    Thread *threads          = NULL;
    int     nthread          = 0;

    nthread = parallel_degree(nrange);
    threads = (Thread *)malloc(nthread * sizeof(Thread));
    memset(threads, 0, nthread * sizeof(Thread));

    for (int index = 0; index < nrange; index++)
    {
        int thread_index             = 2 * index;
        threads[thread_index].rows   = rows;
        threads[thread_index].nrow   = nrow;
        threads[thread_index].range  = ranges + index;
        threads[thread_index].index  = thread_index;
        
        thread_index                 = 2 * index + 1;
        threads[thread_index].rows   = rows;
        threads[thread_index].nrow   = nrow;
        threads[thread_index].range  = ranges + index;
        threads[thread_index].index  = thread_index;
    }

    return threads;
}

/*
 * Figure out the base address and the corresponding upper and lower index
 * of each range with lower and upper row values known.
 *
 * If there exists a range meeting the select condition, then the rows field
 * of the result range points to the given rows of this function, it is not
 * shifted, lower and upper index are corresponding to the given rows.
 *
 * If there does not exist a range, then the rows field of the result range
 * is left NULL to indicate that.
 *
 * Search for each boundary parallelly here.
 */
void
determine_range_boundaries(const Row *rows, int nrow, Range *ranges, int nrange)
{
    Thread *threads          = NULL;
    int     nthread          = parallel_degree(nrange);

    threads = initialize_threads(rows, nrow, ranges, nrange);

    launch_worker_threads(threads, nthread, determine_range_boundary);

    wait_worker_threads(threads, nthread);
}
