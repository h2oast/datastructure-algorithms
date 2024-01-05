#include <stdio.h>

int main(int argc , char **argv)
{
    int  a =   2000;
    int  b =  50000;

    for (int i = 1; i <= a; i++)
    {
        for (int j = 1; j <= b; j++)
        {
            if (i == 10 || i == 200 || i == 3000)
            {
                if (j == 10 || j == 499999)
                    printf("%d,%d\n", i, j);
                else
                    continue;
            }
            else          
            {
                    printf("%d,%d\n", i, j);
            }
                
        }
    }

    return 0;
}