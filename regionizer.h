/*
 *  Created by Tony Lofthouse on 9/18/11.
 *  Copyright 2011 TI. All rights reserved.
 */
#ifndef __REGIONIZER__
#define __REGIONIZER__

typedef struct rect {
    int left, top, right, bottom;
    int blend;
} rect_t;

#define MAXLAYERS 8
#define SUBREGIONMAX ((MAXLAYERS << 1) - 1)

typedef struct hregion {
    rect_t rect;
    int layerids[MAXLAYERS];
    int nlayers;
    int nsubregions;
    rect_t blitrects[MAXLAYERS][SUBREGIONMAX]; /* z-order | rectangle */
} hregion_t;

#endif /* __REGIONIZER__ */
