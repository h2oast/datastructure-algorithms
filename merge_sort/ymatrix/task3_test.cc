#include "row.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static void
load_rows(Row **result_rows, int *result_nrow)
{
    int  nrow = 1000 * 50000;
    Row *rows = (Row *)malloc(nrow * sizeof(Row));

    assert(rows != NULL);

    for (int index = 0; index < nrow; index++)
    {
        Row *row = rows + index;
        int  ret = 0;

        ret = scanf("%d,%d\n", &row->a, &row->b);
        if (ret < 0)
        {
            fprintf(stderr, "read %d rows\n", index);

            *result_rows = rows;
            *result_nrow = index;
            break;
        }
    }
}

int main(int argc, char **argv)
{
    Row *rows = NULL;
    int  nrow = 0;

    load_rows(&rows, &nrow);
    if (nrow <= 0)
    {
        fprintf(stderr, "load rows error: no row is loaded\n");
        return -1;
    }

    task3(rows, nrow);

    return 0;
}