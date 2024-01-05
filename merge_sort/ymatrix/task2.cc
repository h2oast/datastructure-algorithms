#include <stdlib.h>
#include "row.h"

static void
print_range(Range *range)
{
    int        nrows = range->upper_index - range->lower_index;
    const Row *rows  = range->rows + range->lower_index;

    if (range->rows == NULL)
    {
        return;
    }

    for (int index = 0; index < nrows; index++)
    {
        row_printer(rows + index);
    }
}

/*
 * display each non empty range sequentially.
 *
 * maybe this can be parallized later.
 */
static void
print_ranges(Range *ranges, int nrange)
{
    for (int index = 0; index < nrange; index++)
    {
        print_range(ranges + index);
    }
}

/*!
 * Task 2.
 *
 * Find out all the sorted rows that satisfy below conditions:
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
task2(const Row *rows, int nrows)
{    
    Range ranges[] =
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
    const int nrange = 3;

    if ((rows == NULL) || (nrows <= 0))
        return;

    /*
     * figure out boundaries for ranges parallelly
     */    
    determine_range_boundaries(rows, nrows, ranges, nrange);

    /*
     * Output each range parallelly
     */
    print_ranges(ranges, nrange);
}