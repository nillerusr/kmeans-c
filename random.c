#include <stdio.h>
#include "random.h"
#include <stdlib.h>

int random_int(int min, int max)
{
    return rand()%(max+1-min)+min;
}

int compare( const void* a, const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b || int_b == -1 ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

void plus_index(int *c, int maxi, int side)
{
    if( side == 1 )
    {
        if( *c+1 > maxi )
            *c = 0;
        else
            (*c)++;
    }
    else
    {
        if( *c-1 < 0 )
            *c = maxi;
        else
            (*c)--;
    }
}

int *random_sample(int ct, int n)
{
    int *rn = malloc(n*sizeof(int));

    for( int c = 0; c < n; c++)
        rn[c] = -1;

    int randk;
    int rn1 = random_int(0,ct-1);
    int rn2 = random_int(0,ct-1);

    if( rn1 > rn2 )
        rn[0] = random_int(rn2,rn1);
    else
        rn[0] = random_int(rn1,rn2);

    for( int i = 1; i < n; i++)
    {
        qsort( rn, n, sizeof(int), compare );

        randk = random_int(0,i); // генерация рандомных промежутков
        int side = random_int(0,1);

        while(1)
        {
            if( randk == 0 )
            {
                if( rn[0] > 0 )
                {
                    rn[i] = random_int(0,rn[0]-1);
                    break;
                }
                else
                    plus_index(&randk, i, side);
            }
            else if( randk == i )
            {
                if( ct - rn[i-1] > 1 )
                {
                    rn[i] = random_int(rn[i-1]+1, ct-1);
                    break;
                }
                else
                    plus_index(&randk, i, side);
            }
            else
            {
                if( rn[randk] - rn[randk-1] > 1 )
                {
                    rn[i] = random_int(rn[randk-1]+1, rn[randk]-1);
                    break;
                }
                else
                    plus_index(&randk, i, side);
            }
        }
    }

    return rn;
}
