#ifndef ROW_H
#define ROW_H


#define TRUE      1
#define FALSE     0

typedef struct Row
{
    int a;
    int b;
} Row;


extern void
row_printer(const Row *row);

typedef struct Range
{
    Row               lower;       /* lower boundary of the range, including     */
    Row               upper;       /* upper boundary of the range, excluding     */
    const Row        *rows;        /* base of the following indices              */
    int               lower_index; /* the index of the first row of the range
                                    * -1 indicates an invalid index              */
    int               upper_index; /* the index of row exceeding the last row of  
                                      the range                                  */
} Range;

/*
 * return TRUE if left > right as row compare
 * otherwise return FALSE
 */
static inline int
row_compare_gt(const Row *left, const Row *right)
{
    if (left->a > right->a)
    {
        return TRUE;
    } else if (left->a == right->a && left->b > right->b)
    {
        return TRUE;
    }

    return FALSE;
}

/*
 * return TRUE if left >= right as row compare
 * otherwise return FALSE
 */
static inline int
row_compare_ge(const Row *left, const Row *right)
{
    if (row_compare_gt(left, right))
    {
        return TRUE;
    } else if (left->a == right->a && left->b == right->b)
    {
        return TRUE;
    }

    return FALSE;
}


/*
 * return TRUE if left < right as row compare
 * otherwise return FALSE
 */
static inline int
row_compare_lt(const Row *left, const Row *right)
{
    return !row_compare_ge(left, right);
}

/*
 * return TRUE if left <= right as row compare
 * otherwise return FALSE
 */
static inline int
row_compare_le(const Row *left, const Row *right)
{
    return !row_compare_gt(left, right);
}


/*
 * return TRUE if left.b < right.b as row compare
 * otherwise return FALSE
 */
static inline int
row_b_compare_lt(const Row *left, const Row *right)
{
    if (left->b < right->b)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

extern void
determine_range_boundaries(const Row *rows, int nrow, Range *ranges, int nrange);

extern void
task1(const Row *rows, int nrow);

extern void
task2(const Row *rows, int nrow);

extern void
task3(const Row *rows, int nrow);

#endif /* ROW_H */