#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include "row.h"


/*
 * When we merge the ordered rows of each range, we use this iterator.
 */
typedef struct Iterator
{
    Range            *range;       /* the range to be iterated by rows
                                      sequentially                               */
    int               next;        /* the index of next row to be merged in the  
                                      range                                      */
} Iterator;

/*
 * Remove the range reaching its end from the merging list of ranges.
 */
static void
done_iteration_of_range(Iterator *iterators, int niterator, int done_iterator)
{
    for (int index = done_iterator; (index + 1) < niterator; index++)
    {
        iterators[index].range = iterators[index + 1].range;
        iterators[index].next  = iterators[index + 1].next;
    }
}

/*
 * find out the smallest row according to row->b
 */
static void
smallest_row_iterator(Iterator *iterators, int niterator, int *smallest_index)
{
    const Row *smallest_row       = NULL;
    const Row *candidate          = NULL;
    int        smallest           = 0;

    smallest_row = iterators[smallest].range->rows + iterators[smallest].next;

    for (int index = 1; index < niterator; index++)
    {
        candidate = iterators[index].range->rows + iterators[index].next;
        if (row_b_compare_lt(candidate, smallest_row))
        {
            smallest = index;
            smallest_row = iterators[index].range->rows + iterators[index].next;
        }
    }

    *smallest_index = smallest;
}

/*
 * allocate memory for iterators as as continuous array
 */
static void
initialize_iterators(Range *ranges, int nrange, Iterator **iterators, int *niterator)
{
    Iterator *new_iterators = NULL;
    int       count         = 0;

    *niterator = 0;
    new_iterators = (Iterator *)malloc(nrange * sizeof(Iterator));
    memset(new_iterators, 0, nrange * sizeof(Iterator));

    for (int index = 0; index < nrange; index++)
    {
        if (ranges[index].rows != NULL &&
            ranges[index].lower_index < ranges[index].upper_index)
        {
            new_iterators[count].next  = ranges[index].lower_index;
            new_iterators[count].range = ranges + index;

            count++;
        }
    }

    *iterators = new_iterators;
    *niterator = count;
}

/*
 * free memory of iterators
 */
static void
destroy_iterators(Iterator *iterators)
{
    if (iterators)
    {
        free(iterators);
    }
}

/*
 * merge sorting of the ranges that are already sorted by row->b.
 */
static void
merge_ranges(Range *ranges, int nrange, void (*print)(const Row *row))
{
    Iterator *iterators = NULL;
    int       niterator = 0;

    initialize_iterators(ranges, nrange, &iterators, &niterator);

    for (; niterator;)
    {
        Iterator  *iterator           = NULL;
        int        smallest_idx       = 0;
        
        smallest_row_iterator(iterators, niterator, &smallest_idx);
        assert(smallest_idx >= 0 && smallest_idx < niterator);

        iterator = iterators + smallest_idx;
        print(&iterator->range->rows[iterator->next]);
        iterator->next++;

        /*
         * the iterator's range is at its end, remove it from the merging list
         */
        if (iterator->next == iterator->range->upper_index)
        {
            done_iteration_of_range(iterators, niterator, smallest_idx);
            niterator--;
        }
    }
    destroy_iterators(iterators);
    iterators = NULL;
}

/*!
 * Task 3.
 *
 * Find out all the sorted rows that satisfy below conditions:
 *
 *     ((b >= 10 && b < 500000) &&
 *      (a == 10 || a == 200 || a == 3000))
 *
 * Print them to the terminal ordered by b, one row per line, 
 * for example:
 *
 *     200,16
 *     10,23
 *     10,31
 *
 * @param rows  The rows, sorted by (a,b), for example rows[0] is the first row.
 * @param nrows The total number of rows.
 *
 * Idea for this task is that:
 *     1. find out boundaries for the three ranges parallelly: 
 *         [(  10, 10), (  10, 500000))
 *         [( 200, 10), ( 200, 500000))
 *         [(3000, 10), (3000, 500000))
 *     2. print the three ranges parallelly, with parellel degree 2 * ncpu,
 *        for each range use the parallel method as task1 but just print with-
 *        out check the filter conditions. Rows of the three ranges naturally
 *        satisfy the search conditions due to the sorted assumption.
 */
void
task3(const Row *rows, int nrows)
{    
    const int nrange = 3;
    Range ranges[]   =
    {
        /*
        * range of the select condition a = 10 && b >= 10 && b < 500000
        */
        {
            .lower       = {.a = 10, .  b =     10},
            .upper       = {.a = 10, .  b = 500000},
            .rows        = NULL,
            .lower_index = -1,
            .upper_index = -1,
        },

        /*
        * range of the select condition a = 200 && b >= 10 && b < 500000
        */
        {
            .lower       = {.a = 200, . b =     10},
            .upper       = {.a = 200, . b = 500000},
            .rows        = NULL,
            .lower_index = -1,
            .upper_index = -1,
        },

        /*
        * range of the select condition a = 3000 && b >= 10 && b < 500000
        */
        {
            .lower       = {.a = 3000, .b =     10},
            .upper       = {.a = 3000, .b = 500000},
            .rows        = NULL,
            .lower_index = -1,
            .upper_index = -1,
        },
    };

    if ((rows == NULL) || (nrows <= 0))
        return;

    /*
     * figure out boundaries for ranges parallelly
     */    
    determine_range_boundaries(rows, nrows, ranges, nrange);

    /*
     * Merge the three ranges with each ordered by b, but not totally ordered
     * among the three ranges
     */
    merge_ranges(ranges, nrange, row_printer);
}