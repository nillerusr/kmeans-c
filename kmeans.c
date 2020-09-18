//
// Created by nillerusr on 17.12.19.
//

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "random.h"
#include "sqlite3.h"
#include "images.h"
#include "kmeans.h"

double max2 ( double a, double b ) { return a > b ? a : b; }

double euclid(point_t *p1, point_t *p2) // Calculate Euclidian distance
{
    double sum = 0;

    for(int i = 0; i < 3; i++)
        sum += pow(p1->color[i] - p2->color[i],2);
    return pow(sum, 0.5);
}

point_t calculate_center( point_t *p, int n ) // calculate new center for cluster
{
    point_t ret;
    double vals[3];
    int plen = 0;

    for(int i = 0; i < 3; i++)
        vals[i] = 0;

    for(int i = 0; i < n; i++)
    {
        plen += p[i].count;

        for(int c = 0; c < 3; c++)
            vals[c] += (p[i].color[c]*p[i].count);
    }

    for(int i = 0; i < 3; i++)
        ret.color[i] = vals[i] / plen;

    ret.count = 1;
    return ret;
}

int *kmeans(image_t *img, int k) // k-means algorythm
{
    int *cls;
    int *ret = (int*)malloc(3*sizeof(int));
    int idx, i, c, mx;
    int maxi[2];

    cluster_t *clusters = (cluster_t*)malloc(k * sizeof(cluster_t));

    for( i = 0; i < 3; i++)
        ret[i] = 0;

    if(k > img->count)
        k = img->count;

    cls  = random_sample(img->count, k); // randomly choice k points

    for(i = 0; i < k; i++) // add choiced points to clusters
    {
        clusters[i].plist = (point_t*)malloc(img->count*sizeof(point_t));
        clusters[i].center = img->points[cls[i]];
    }

    while(1)
    {
        for(c = 0; c < k; c++)
        clusters[c].pcount = 0;

        for(i = 0; i < img->count; i++) // scan all points and find the nearest point
        {
            double smallest_dist = 10e6;
            for(c = 0; c < k; c++ )
            {
                double dist = euclid(&img->points[i], &clusters[c].center );
                if( dist < smallest_dist )
                {
                    smallest_dist = dist;
                    idx = c;
                }
            }
            clusters[idx].plist[clusters[idx].pcount] = img->points[i]; // add nearest point to cluster
            clusters[idx].pcount++;
        }

        double diff = 0;
        for(i = 0; i < k; i++)
        {
            point_t old_center = clusters[i].center;
            clusters[i].center = calculate_center(clusters[i].plist, clusters[i].pcount); // calculate new center
            diff = max2(diff, euclid(&old_center, &clusters[i].center)); // Finding maximum distance between new and previous centers
        }

        if( diff < 1 ) // If distance too small do break
            break;
    }

    if( k >= 2 )
    {
        for (c = 0; c < 2; c++)
        {
            mx = clusters[0].pcount;
            maxi[c] = 0;
            for (i = 1; i < k; i++)
            {
                if (mx < clusters[i].pcount)
                {
                    mx = clusters[i].pcount;
                    maxi[c] = i;
                }
            }
            clusters[maxi[c]].pcount = 0;
        }


        for (i = 0; i < 2; i++)
        {
            ret[0] += (int) clusters[maxi[i]].center.color[0];
            ret[1] += (int) clusters[maxi[i]].center.color[1];
            ret[2] += (int) clusters[maxi[i]].center.color[2];
        }
        ret[0] /= 2; ret[1] /= 2; ret[2] /= 2; // Calculating medium color
    }
    else
    {
        ret[0] = (int) clusters[0].center.color[0];
        ret[1] = (int) clusters[0].center.color[1];
        ret[2] = (int) clusters[0].center.color[2];
    }

    for (i = 0; i < k; i++)
        free(clusters[i].plist);
    free(img->points);
    free(img);
    free(clusters);
    free(cls);

    return ret;
}


