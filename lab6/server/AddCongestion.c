#include <stdio.h>
#include <stdlib.h>

void AddCongestion(double p)
{
    if( p < rand() % 100 )
    {
        return 1;
    }
    return 0;
}