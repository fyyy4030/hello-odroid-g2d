/* hello-world-g2d.c
 *
 * Copyright (c) 2013 neagix
 *
 * example for Samsung Exynos4 G2D direct usage
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <stdio.h>
#include <stdlib.h>

#include "bool.h"

#include "fimg4x.h"
#include "fimglib.h"

#include <memory.h>
#include <unistd.h>

Fimg fimg;

int main()
{
    printf("Hello G2D world!\n");

    // some random video data
    unsigned char videoBuffer[50*50*2];
    memset(videoBuffer, 0x6A, sizeof(videoBuffer));

    fimg.srcAddr = videoBuffer;
    fimg.srcBPP = 2;

    fimg.dstBPP = 24/8;
    fimg.srcColorFormat = CF_RGB_565;
    fimg.dstColorFormat = CF_RGB_565;
    fimg.dstFWStride = 1024 * fimg.dstBPP;
    fimg.dstFH = 768;

    if (fimg_init() == false)
    {
        fprintf(stderr, "No FIMG API, aborting\n");
        return -1;
    }
    atexit(&fimg_deinit);


    fimg.matrixSx = 1;
    fimg.matrixSy = 1;

    fimg.srcFWStride    = 50 * fimg.srcBPP;
    fimg.srcFH          = 50;

    fimg.rotate         = 0;

    fimg.alpha = 255;


    fimg.srcX = 0;
    fimg.srcY = 0;
    fimg.srcW = 50;
    fimg.srcH = 50;
    fimg.dstX           = 100;
    fimg.dstY           = 100;
    fimg.dstW           = fimg.srcW;
    fimg.dstH           = fimg.srcH;

    fimg.clipT = fimg.dstY;
    fimg.clipB = fimg.clipT + fimg.dstW;
    fimg.clipL = fimg.dstX;
    fimg.clipR = fimg.clipL + fimg.dstW;

    fimg.matrixType = kIdentity_Mask;

    fimg.xfermode = BLIT_OP_SRC;
    fimg.isDither = false;

    fimg.scalingMode = SCALING_NEAREST; //SCALING_BILINEAR
    fimg.colorFilter = 0x0;

    if ((fimg.dstX<0)||(fimg.dstW<=0)||(fimg.dstH<=0))
        fimg.srcAddr = NULL;

    if (fimg.srcAddr != NULL)
    {
        int i;
        for(i=0; i<100; i++)
        {
            if (fimg_stretch_draw(&fimg, __func__))
            {
                printf("Success!\n");
                fimg_sync();
                sleep(1);
                //fimg.srcAddr = NULL;
                //gG2DMutex.release();

            }
            else
            {
                printf("fail\n");
                break;
            }
        }
    }

    fimg.srcAddr = NULL;
    //gG2DMutex.release();


    printf("Exiting\n");
    fflush(stdout);

    //fimg_deinit() is run automatically at exit
    return 0;
}
