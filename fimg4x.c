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

#include "fimg4x.h"

#define DEBUG_PRINT   printf

struct blit_op_table
{
    int op;
    const char *str;
};

static struct blit_op_table optbl[] =
{
    { (int)BLIT_OP_SOLID_FILL, "FILL" },
    { (int)BLIT_OP_CLR, "CLR" },
    { (int)BLIT_OP_SRC, "SRC" },
    { (int)BLIT_OP_DST, "DST" },
    { (int)BLIT_OP_SRC_OVER, "SRC_OVER" },
    { (int)BLIT_OP_DST_OVER, "DST_OVER" },
    { (int)BLIT_OP_SRC_IN, "SRC_IN" },
    { (int)BLIT_OP_DST_IN, "DST_IN" },
    { (int)BLIT_OP_SRC_OUT, "SRC_OUT" },
    { (int)BLIT_OP_DST_OUT, "DST_OUT" },
    { (int)BLIT_OP_SRC_ATOP, "SRC_ATOP" },
    { (int)BLIT_OP_DST_ATOP, "DST_ATOP" },
    { (int)BLIT_OP_XOR, "XOR" },
    { (int)BLIT_OP_ADD, "ADD" },
    { (int)BLIT_OP_MULTIPLY, "MULTIPLY" },
    { (int)BLIT_OP_SCREEN, "SCREEN" },
    { (int)BLIT_OP_DARKEN, "DARKEN" },
    { (int)BLIT_OP_LIGHTEN, "LIGHTEN" },
    { (int)BLIT_OP_DISJ_SRC_OVER, "DISJ_SRC_OVER" },
    { (int)BLIT_OP_DISJ_DST_OVER, "DISJ_DST_OVER" },
    { (int)BLIT_OP_DISJ_SRC_IN, "DISJ_SRC_IN" },
    { (int)BLIT_OP_DISJ_DST_IN, "DISJ_DST_IN" },
    { (int)BLIT_OP_DISJ_SRC_OUT, "DISJ_SRC_OUT" },
    { (int)BLIT_OP_DISJ_DST_OUT, "DISJ_DST_OUT" },
    { (int)BLIT_OP_DISJ_SRC_ATOP, "DISJ_SRC_ATOP" },
    { (int)BLIT_OP_DISJ_DST_ATOP, "DISJ_DST_ATOP" },
    { (int)BLIT_OP_DISJ_XOR, "DISJ_XOR" },
    { (int)BLIT_OP_CONJ_SRC_OVER, "CONJ_SRC_OVER" },
    { (int)BLIT_OP_CONJ_DST_OVER, "CONJ_DST_OVER" },
    { (int)BLIT_OP_CONJ_SRC_IN, "CONJ_SRC_IN" },
    { (int)BLIT_OP_CONJ_DST_IN, "CONJ_DST_IN" },
    { (int)BLIT_OP_CONJ_SRC_OUT, "CONJ_SRC_OUT" },
    { (int)BLIT_OP_CONJ_DST_OUT, "CONJ_DST_OUT" },
    { (int)BLIT_OP_CONJ_SRC_ATOP, "CONJ_SRC_ATOP" },
    { (int)BLIT_OP_CONJ_DST_ATOP, "CONJ_DST_ATOP" },
    { (int)BLIT_OP_CONJ_XOR, "CONJ_XOR" },
    { (int)BLIT_OP_USER_COEFF, "USER_COEFF" },
    { (int)BLIT_OP_END, "" },
};

static void printDataBlitImage(char *title, struct fimg2d_image *image)
{
    if (NULL != image)
    {
        DEBUG_PRINT("    Image_%s\n", title);
        DEBUG_PRINT("        addr = %lX\n", image->addr.start);
        DEBUG_PRINT("        size = n.a.\n" /*, image->addr.size*/);
        DEBUG_PRINT("        (width, height) = (%d, %d)\n", image->width, image->height);
        DEBUG_PRINT("        format = %d\n", image->fmt);
    }
    else
        DEBUG_PRINT("    Image_%s : NULL\n", title);
}

static void printDataBlitRect(char *title, struct fimg2d_rect *rect)
{
    if (NULL != rect)
    {
        DEBUG_PRINT("    RECT_%s\n", title);
        DEBUG_PRINT("        (x1, y1) = (%d, %d)\n", rect->x1, rect->y1);
        DEBUG_PRINT("        (x2, y2) = (%d, %d)\n", rect->x2, rect->y2);
        DEBUG_PRINT("        (width, height) = (%d, %d)\n", rect->x2 - rect->x1, rect->y2 - rect->y1);
    }
    else
        DEBUG_PRINT("    RECT_%s : NULL\n", title);
}

static void printDataBlitRotate(enum rotation rotate)
{
    DEBUG_PRINT("    ROTATE : %d\n", rotate);
}

static void printDataBlitClip(struct fimg2d_clip *clip)
{
    if (NULL != clip)
    {
        DEBUG_PRINT("    CLIP\n");
        DEBUG_PRINT("        clip %s\n", clip->enable == true ? "enabled" : "disabled");
        DEBUG_PRINT("        (x1, y1) = (%d, %d)\n", clip->x1, clip->y1);
        DEBUG_PRINT("        (x2, y2) = (%d, %d)\n", clip->x2, clip->y2);
        DEBUG_PRINT("        (width, hight) = (%d, %d)\n", clip->x2 - clip->x1, clip->y2 - clip->y1);
    }
    else
        DEBUG_PRINT("    CLIP : NULL\n");
}

static void printDataBlitScale(struct fimg2d_scale *scaling)
{
    if (NULL != scaling)
    {
        DEBUG_PRINT("    SCALING\n");
        DEBUG_PRINT("        scale_mode : %s\n", scaling->mode == 0 ?
              "NO_SCALING" :
              (scaling->mode == 1 ? "SCALING_NEAREST" : "SCALING_BILINEAR"));
        DEBUG_PRINT("        scaling_factor_unit : %s\n", /*scaling->factor*/ 1 == 0 ? "PERCENT" : "PIXEL");

        /*        if (scaling->factor == 0)
                    DEBUG_PRINT("        scaling_factor : (scale_w, scale_y) = (%d, %d)\n", scaling->scale_w, scaling->scale_h);
                else */
        {
            DEBUG_PRINT("        src : (src_w, src_h) = (%d, %d)\n", scaling->src_w, scaling->src_h);
            DEBUG_PRINT("        dst : (dst_w, dst_h) = (%d, %d)\n", scaling->dst_w, scaling->dst_h);
            DEBUG_PRINT("        scaling_factor : (scale_w, scale_y) = (%3.2f, %3.2f)\n", (double)scaling->dst_w / scaling->src_w, (double)scaling->dst_h / scaling->src_h);
        }
    }
    else
        DEBUG_PRINT("    SCALING : NULL(NO SCALE MODE)\n");

}

static void printDataMatrix(int matrixType)
{
    DEBUG_PRINT("    MATRIX\n");

    if (matrixType & kIdentity_Mask)
        DEBUG_PRINT("        Matrix_type : Identity_Mask\n");

    if (matrixType & kTranslate_Mask)
        DEBUG_PRINT("        Matrix_type : Translate_Mask(the matrix has translation)\n");

    if (matrixType & kScale_Mask)
        DEBUG_PRINT("        Matrix_type : Scale_Mask(the matrix has X or Y scale)\n");

    if (matrixType & kAffine_Mask)
        DEBUG_PRINT("        Matrix_type : Affine_Mask(the matrix skews or rotates)\n");

    if (matrixType & kPerspective_Mask)
        DEBUG_PRINT("        Matrix_type : Perspective_Mask(the matrix is in perspective)\n");
}

static void printDataBlit(char *title, struct fimg2d_blit *cmd)
{
    DEBUG_PRINT("%s\n", title);

    DEBUG_PRINT("    sequence_no. = %u\n", cmd->seq_no);
    DEBUG_PRINT("    blit_op      = %d(%s)\n", cmd->op, optbl[cmd->op].str);
    DEBUG_PRINT("    fill_color   = %lX\n", cmd->param.solid_color);
    DEBUG_PRINT("    global_alpha = %u\n", (unsigned int)cmd->param.g_alpha);
    DEBUG_PRINT("    PREMULT      = %s\n", cmd->param.premult == PREMULTIPLIED ? "PREMULTIPLIED" : "NON-PREMULTIPLIED");
    DEBUG_PRINT("    do_dither    = %s\n", cmd->param.dither == true ? "dither" : "no-dither");

    printDataBlitRotate(cmd->param.rotate);

    printDataBlitScale(&cmd->param.scaling);

    printDataBlitImage("SRC", cmd->src);
    printDataBlitImage("DST", cmd->dst);
    printDataBlitImage("MSK", cmd->msk);

    printDataBlitRect("SRC", &cmd->src->rect);
    if (cmd->dst != NULL)
        printDataBlitRect("DST", &cmd->dst->rect);
    if (cmd->msk != NULL)
        printDataBlitRect("MSK", &cmd->msk->rect);

    printDataBlitClip(&cmd->param.clipping);
}

static bool IsDstMode(Fimg *fimg)
{
    if (fimg->xfermode == BLIT_OP_DST)
        return true;
    else
        return false;
}

bool CheckPossible(Fimg *fimg)
{
    if (fimg->srcAddr != NULL)
    {
        switch (fimg->srcColorFormat)
        {
        case CF_RGB_565:
        case CF_ARGB_8888:
            break;
        default:
            return false;
        }
    }

    switch (fimg->dstColorFormat)
    {
    case CF_RGB_565:
        if ((fimg->srcColorFormat == CF_ARGB_8888) &&
                (fimg->isDither == true))
            return false;
        break;
    case CF_ARGB_8888:
        break;
    default:
        return false;
    }

    switch (fimg->xfermode)
    {
    case BLIT_OP_SRC_OVER:
    case BLIT_OP_CLR:
    case BLIT_OP_SRC:
    case BLIT_OP_DST:
        break;
    default:
        return false;
    }

    if (fimg->colorFilter != 0)
        return false;

    if (fimg->matrixType & kAffine_Mask)
        return false;

    if ((fimg->matrixSx < 0) || (fimg->matrixSy < 0))
        return false;

    if ((fimg->srcX + fimg->srcW) > 8000 || (fimg->srcY + fimg->srcH) > 8000)
        return false;

    if ((fimg->dstX + fimg->dstW) > 8000 || (fimg->dstY + fimg->dstH) > 8000)
        return false;

    if ((fimg->clipT < 0) || (fimg->clipB < 0) || (fimg->clipL < 0) || (fimg->clipR < 0))
    {
        DEBUG_PRINT("Invalid clip value: TBLR = (%d, %d, %d, %d)",fimg->clipT, fimg->clipB, fimg->clipL, fimg->clipR);
        return false;
    }

    if ((fimg->clipT >= fimg->clipB) || (fimg->clipL >= fimg->clipR))
    {
        DEBUG_PRINT("Invalid clip value: TBLR = (%d, %d, %d, %d)",fimg->clipT, fimg->clipB, fimg->clipL, fimg->clipR);
        return false;
    }

    return true;
}

static bool CheckPossible_Clipping(Fimg *fimg)
{
    if (((fimg->clipR - fimg->clipL) <= 0) || ((fimg->clipB - fimg->clipT) <= 0))
        return false;

    return true;
}

int fimg_stretch_draw(Fimg *fimg, const char *func_name)
{
    static unsigned int seq_no = 100;

    struct fimg2d_blit cmd;
    struct fimg2d_image srcImage;
    struct fimg2d_image dstImage;

    /* to support negative Y coordinate */
    if ((fimg->dstAddr != NULL) && (fimg->dstY < 0))
    {
        if (fimg->dstH > fimg->dstFH)
            fimg->dstFH = fimg->dstH;
        fimg->dstAddr += fimg->dstFWStride * fimg->dstY;
        fimg->clipT -= fimg->dstY;
        fimg->clipB -= fimg->dstY;
        fimg->dstY = 0;
    }

    if (CheckPossible(fimg) == false)
        return false;

    if (IsDstMode(fimg) == true)
        return FIMGAPI_FINISHED;

    if (fimg->clipL < fimg->dstX)
        fimg->clipL = fimg->dstX;
    if (fimg->clipT < fimg->dstY)
        fimg->clipT = fimg->dstY;
    if (fimg->clipR > (fimg->dstX + fimg->dstW))
        fimg->clipR = fimg->dstX + fimg->dstW;
    if (fimg->clipB > (fimg->dstY + fimg->dstH))
        fimg->clipB = fimg->dstY + fimg->dstH;

#if FIMGAPI_COMPROMISE_USE
    if (Compromise(fimg) == false)
        return false;
#endif
    enum rotation rotate;

    switch (fimg->rotate)
    {
    case 0:
        rotate = ORIGIN;
        break;
    case 90:
        rotate = ROT_90;
        break;
    case 180:
        rotate = ROT_180;
        break;
    case 270:
        rotate = ROT_270;
        break;
    default:
        return false;
    }

    cmd.op = fimg->xfermode;
    cmd.param.g_alpha = fimg->alpha;
    cmd.param.premult = PREMULTIPLIED;
    cmd.param.dither = fimg->isDither;
    cmd.param.rotate = rotate;
    cmd.param.solid_color = fimg->fillcolor;

    if (fimg->srcAddr != NULL && (fimg->srcW != fimg->dstW || fimg->srcH != fimg->dstH))
    {
        cmd.param.scaling.mode = fimg->scalingMode;
        cmd.param.scaling.src_w = fimg->srcW;
        cmd.param.scaling.src_h = fimg->srcH;
        cmd.param.scaling.dst_w = fimg->dstW;
        cmd.param.scaling.dst_h = fimg->dstH;
    }
    else
        cmd.param.scaling.mode = NO_SCALING;

    cmd.param.repeat.mode = NO_REPEAT;
    cmd.param.repeat.pad_color = 0x0;

    cmd.param.bluscr.mode = OPAQUE;
    cmd.param.bluscr.bs_color = 0x0;
    cmd.param.bluscr.bg_color = 0x0;

    if (fimg->srcAddr != NULL)
    {
        srcImage.addr.type = ADDR_USER;
        srcImage.addr.start = (long unsigned)fimg->srcAddr;
        srcImage.need_cacheopr = true;
        srcImage.width = fimg->srcFWStride / fimg->srcBPP;
        srcImage.height = fimg->srcFH;
        srcImage.stride = fimg->srcFWStride;
        if (fimg->srcColorFormat == CF_RGB_565)
            srcImage.order = AX_RGB;
        else
            srcImage.order = AX_BGR; // kARGB_8888_Config

        srcImage.fmt = fimg->srcColorFormat;
        srcImage.rect.x1 = fimg->srcX;
        srcImage.rect.y1 = fimg->srcY;
        srcImage.rect.x2 = fimg->srcX + fimg->srcW;
        srcImage.rect.y2 = fimg->srcY + fimg->srcH;
        cmd.src = &srcImage;
    }
    else
        cmd.src = NULL;

    if (fimg->dstAddr != NULL)
    {
        dstImage.addr.type = ADDR_USER;
        dstImage.addr.start = (long unsigned)fimg->dstAddr;
        dstImage.need_cacheopr = true;
        dstImage.width = fimg->dstFWStride / fimg->dstBPP;
        dstImage.height = fimg->dstFH;
        dstImage.stride = fimg->dstFWStride;
        if (fimg->dstColorFormat == CF_RGB_565)
            dstImage.order = AX_RGB;
        else
            dstImage.order = AX_BGR; // kARGB_8888_Config

        dstImage.fmt = fimg->dstColorFormat;
        dstImage.rect.x1 = fimg->dstX;
        dstImage.rect.y1 = fimg->dstY;
        dstImage.rect.x2 = fimg->dstX + fimg->dstW;
        dstImage.rect.y2 = fimg->dstY + fimg->dstH;

        cmd.param.clipping.enable = true;
        cmd.param.clipping.x1 = fimg->clipL;
        cmd.param.clipping.y1 = fimg->clipT;
        cmd.param.clipping.x2 = fimg->clipR;
        cmd.param.clipping.y2 = fimg->clipB;

        cmd.dst = &dstImage;

    }
    else
        cmd.dst = NULL;

    cmd.msk = NULL;

    cmd.tmp = NULL;
    cmd.sync = BLIT_SYNC;
    cmd.seq_no = seq_no++;

    if (CheckPossible_Clipping(fimg) == false)
    {
        return false;
    }

#if defined(FIMGAPI_DEBUG_MESSAGE)
    printDataBlit("Before stretch:", &cmd);
    printDataMatrix(fimg->matrixType);
#endif

    if (fimg_stretch_internal(&cmd) == false)
    {
#if defined(FIMGAPI_DEBUG_MESSAGE)
        DEBUG_PRINT("%s:stretch failed\n", __FUNCTION__);
#endif
        return false;
    }

    return FIMGAPI_FINISHED;
}

