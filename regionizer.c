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
#include <strings.h>

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

/*
 * Simple bubble sort on an array
 */
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

/*
 * Leave only unique numbers in a sorted array
 */
static int bunique(int *a, int len)
{
    int unique = 1;
    int base=0;
    while (base + 1 < len) {
        if (a[base] == a[base + 1]) {
            int skip = 1;
            while (base + skip < len && a[base] == a[base + skip])
                skip++;
            if (base + skip == len)
                break;
            for (int i = 0; i < skip - 1; i++)
                a[base + 1 + i] = a[base + skip];
        }
        unique++;
        base++;
    }
    return unique;
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
    {0, 0, 640, 480, 0},
    {0, 0, 640, 40, 0},
    {0, 400, 640, 480, 0},
    {440, 280, 520, 360, 1}, /* dialog w/ blending */
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
    int layerids[NUMLAYERS];
    int nlayers;
    rect_t blitrects[NUMLAYERS][NUMLAYERS]; /* z-order | rectangle */
} hregion_t;

static void gen_blitregions(hregion_t *hregion, rect_t *layers)
{
/*
 * 1. Get the offsets (left/right positions) of each layer within the
 *    hregion. Assume that layers describe the bounds of the hregion.
 * 2. We should then be able to generate an array of rects
 * 3. Each layer will have a different z-order, for each z-order
 *    find the intersection. Some intersections will be empty.
 */

    int offsets[KMAX];
    int noffsets=0;
    for (int l = 0; l < hregion->nlayers; l++) {
        int layeridx = hregion->layerids[l];
        offsets[noffsets++] = layers[layeridx].left;
        offsets[noffsets++] = layers[layeridx].right;
    }
    bsort(offsets, noffsets);
    noffsets = bunique(offsets, noffsets);

    bzero(hregion->blitrects, sizeof(hregion->blitrects));
    for (int r = 0; r + 1 < noffsets; r++) {
        rect_t subregion;
        subregion.blend = 0;
        subregion.top = hregion->rect.top;
        subregion.bottom = hregion->rect.bottom;
        subregion.left = offsets[r];
        subregion.right = offsets[r+1];

        for (int l = 0; l < hregion->nlayers; l++) {
            int layeridx = hregion->layerids[l];
            if (intersects(&layers[layeridx], &subregion)) {

                subregion.blend = layers[layeridx].blend;
                hregion->blitrects[layeridx][r] = subregion;
                
                printf("Intersect layer (z): %d x (%d %d %d %d) (blend: %s)\n",
                       layeridx, subregion.left, subregion.top,
                       subregion.right, subregion.bottom, subregion.blend ? "yes" : "no"); // XXX
            }
        }

    }
}

int main (int argc, const char * argv[])
{
#if 0
    // test code
    int array[6][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 8, 4, 7, 9},
    {0, 1, 0, 1, 0, 1, 0, 1},
    {3, 5, 2, 1, 8, 4, 7, 9},
    {3, 3, 2, 1, 8, 4, 7, 9},
    {3, 5, 5, 1, 8, 4, 7, 9}
    };
    for (int i = 0; i < 6; i++) {
        int arraysz = sizeof(array[i]) / sizeof(int);
        printarray(array[i], arraysz);
        bsort(array[i], arraysz);
        printarray(array[i], arraysz);

        int sz = bunique(array[i], arraysz);
        printf("sz %d arraysz %d\n", sz, arraysz);
        printarray(array[i], sz);
    }
//    exit(0);  // XXX
#endif

    // the real stuff
    assert(layerno <= KMAX);
    int yentries[KMAX];
    
    // Find the horizontal regions
    int ylen = rect_sortbyy(layers, layerno, yentries);
    //printarray(yentries, ylen);
    
    ylen = bunique(yentries, ylen);
    //printarray(yentries, ylen);

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
                hregions[i].layerids[l] = j;
            }
        }
    }

    /* Calculate blit regions */
    for (int i = 0; i < nhregions; i++) {
        printf("layers between %d and %d: ", hregions[i].rect.top, hregions[i].rect.bottom);
        for (int j = 0; j < hregions[i].nlayers; j++)
            printf("%d ", hregions[i].layerids[j]);
        printf("\n");

        gen_blitregions(&hregions[i], layers);
    }

    return 0;
}

