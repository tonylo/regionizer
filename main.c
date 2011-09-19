//
//  main.c
//  rectangles
//
//  Created by Tony Lofthouse on 9/18/11.
//  Copyright 2011 TI. All rights reserved.
//

#include <stdio.h>
#include <time.h>
#include <assert.h>

#define NUMLAYERS 8
#define KMAX (NUMLAYERS << 1)

static int bswap(int *a, int *b)
{
    if (*a > *b) {
        int tmp = *b;
        *b = *a;
        *a = tmp;
        return 1;
    }
    return 0;
}

static void bsort(int *a, int len)
{
    int i, s;

    do {
        s=0;
        for (i=0; i+1<len; i++) {
            if (bswap(&a[i], &a[i+1]))
                s = 1;
        }
    } while (s);
}

static int bunique(int *a, int len)
{
    int unique = 0;
    int i = 1;
    
    while(i++ < len) {
        if (a[unique] != a[i]) {
            unique++;
            a[unique] = a[i];
        }
    }
    return unique ? unique : 1;
}

static void printarray(int *a, int len)
{
    for (int i=0; i<len; i++)
        printf("%d ", a[i]);
    printf("\n");
}

static void timestamp(void)
{
    clock_t t = clock();
    printf("time = %lu\n", t / (CLOCKS_PER_SEC / 1000000));
}

typedef struct rect {
    int left, top, right, bottom;
} rect_t;

rect_t rects[] = {
    {0, 0, 640, 480},
    {0, 0, 640, 80},
    {0, 560, 640, 640},
    {440, 280, 360, 520},
};
int rectno = sizeof(rects)/sizeof(rect_t);

static int rect_sortbyy(rect_t *ra, int rsz, int *out)
{
    int outsz=0;
    for (int i=0; i < rsz; i++) {
        out[outsz++] = ra[i].top;
        out[outsz++] = ra[i].bottom;
    }
    bsort(out, outsz);
    return outsz;
}

int main (int argc, const char * argv[])
{

    // insert code here...
    printf("Hello, World!\n");
    
    // test code
    int array[] = {3, 5, 2, 1, 8, 4, 7, 9};
    int arraysz = sizeof(array) / sizeof(int);
    printarray(array, arraysz);
    timestamp();
    bsort(array, arraysz);
    timestamp();
    printarray(array, arraysz);
    
    // the real stuff
    assert(rectno <= KMAX);
    int yentries[KMAX];
    
    int ylen;
    timestamp();
    // Find the vertical regions
    ylen = rect_sortbyy(rects, rectno, yentries);
    timestamp();
    printarray(yentries, ylen);
    
    ylen = bunique(yentries, ylen);
    printarray(yentries, ylen);

    // at this point we have an array of vertical regions
    return 0;
}

