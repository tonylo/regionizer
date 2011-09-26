/*
 *  Created by Tony Lofthouse on 9/18/11.
 *  Copyright 2011 TI. All rights reserved.
 */
#ifndef __REGIONIZER__
#define __REGIONIZER__

typedef struct rect {
    int left, top, right, bottom;
    int blend; /* TODO */
} rect_t;

#define NUMLAYERS 8
#define KMAX (NUMLAYERS << 1)

typedef struct hregion {
    rect_t rect;
    int layerids[NUMLAYERS];
    int nlayers;
    rect_t blitrects[NUMLAYERS][NUMLAYERS]; /* z-order | rectangle */
} hregion_t;

#endif /* __REGIONIZER__ */
