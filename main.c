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
    int blend; /* TODO */
} rect_t;

rect_t layers[] = {
    {0, 0, 640, 480},
    {0, 0, 640, 40},
    {0, 400, 640, 480},
    {440, 280, 520, 360},
};
int layerno = sizeof(layers)/sizeof(rect_t);

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

static int intersects(rect_t *a, rect_t *b)
{
    return ((a->bottom > b->top) && (a->top < b->bottom) &&
            (a->right > b->left) && (a->left < b->right));
}

typedef struct hregion {
    rect_t rect;
    int layers[NUMLAYERS];
    int nlayers;
    rect_t blitrects[NUMLAYERS][NUMLAYERS]; /* z-order | rectangle */
} hregion_t;


static void gen_blitregions(hregion_t *hregion)
{
/*
 * 1. Crop the layers to the bounds of the hregion (top / bottom)
 * 2. Find the right position of each layer, add 0 and sort them from
 *    high to low
 * 3. We should then be able to generate an array of rects
 * 4. Each layer will have a different z-order, for each z-order
 *    find the intersection. Some intersections will be empty.
 */
    int crop_layers[hregion->nlayers];
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
    assert(layerno <= KMAX);
    int yentries[KMAX];
    
    int ylen;
    timestamp();
    // Find the vertical regions
    ylen = rect_sortbyy(layers, layerno, yentries);
    timestamp();
    printarray(yentries, ylen);
    
    ylen = bunique(yentries, ylen);
    printarray(yentries, ylen);

    // at this point we have an array of horizontal regions
    int nhregions = ylen - 1;
    int dispw = 640; /* XXX should obtain this from somewhere */
    hregion_t hregions[KMAX];
    for (int i = 0; i < nhregions; i++) {
        hregions[i].rect.top = yentries[i];
        hregions[i].rect.bottom = yentries[i+1];
        hregions[i].rect.left = 0;
        hregions[i].rect.right = dispw;
        hregions[i].nlayers = 0;
        for (int j = 0; j < layerno; j++) {
            if (intersects(&hregions[i].rect, &layers[j])) {
                int l = hregions[i].nlayers++;
                hregions[i].layers[l] = j;
            }
        }
    }

    // print intersecting layers
    for (int i = 0; i < nhregions; i++) {
	printf("layers between %d and %d: ", hregions[i].rect.top, hregions[i].rect.bottom);
	for (int j = 0; j < hregions[i].nlayers; j++) {
             printf("%d ", hregions[i].layers[j]);
        }
	printf("\n");
    }

    return 0;
}

