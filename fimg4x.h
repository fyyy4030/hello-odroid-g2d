/*
**
** Copyright 2013 neagix
** Copyright 2013, Daniel Hillenbrand, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
**
*/

#ifndef __FIMG4X_H
#define __FIMG4X_H

#include "bool.h"

#include "fimglib.h"

// copied from 3.0.84 kernel
#include "fimg2d.h"

#include <stdio.h>


#define FIMGAPI_FINISHED       (0x1<<0)

typedef struct
{
    int            srcX;
    int            srcY;
    unsigned int   srcW;
    unsigned int   srcH;
    unsigned int   srcFWStride; // this is not w, just stride (w * bpp)
    unsigned int   srcFH;
    unsigned int   srcBPP;
    int            srcColorFormat;
    unsigned char *srcAddr;

    int            dstX;
    int            dstY;
    unsigned int   dstW;
    unsigned int   dstH;
    unsigned int   dstFWStride; // this is not w, just stride (w * bpp)
    unsigned int   dstFH;
    unsigned int   dstBPP;
    int            dstColorFormat;
    unsigned char *dstAddr;

    int            clipT;
    int            clipB;
    int            clipL;
    int            clipR;

    int            rotate;
    unsigned long  fillcolor;
    int            alpha;
    int            xfermode;
    int            isDither;
    int            scalingMode;
    int            colorFilter;
    int            canusehybrid;
    int            matrixType;
    int            matrixSx;
    int            matrixSy;
} Fimg;

enum TypeMask
{
    kIdentity_Mask      = 0,
    kTranslate_Mask     = 0x01,  //!< set if the matrix has translation
    kScale_Mask         = 0x02,  //!< set if the matrix has X or Y scale
    kAffine_Mask        = 0x04,  //!< set if the matrix skews or rotates
    kPerspective_Mask   = 0x08   //!< set if the matrix is in perspective
};

int fimg_stretch_draw(Fimg *fimg, const char *func_name);

#endif // __FIMG4X_H
