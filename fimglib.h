/* fimglib.h
 *
 * Copyright (c) 2013 neagix
 *
 * fimg access library
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef	__FIMGLIB_H
#define	__FIMGLIB_H

#include "bool.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/stat.h>

#include <pthread.h>
#include <memory.h>
#include <stdio.h>

// kernel structures
#include "fimg2d.h"

int fimg_stretch_internal(struct fimg2d_blit *cmd);
bool fimg_sync(void);
bool fimg_init();
void fimg_deinit();

#endif // __FIMGLIB_H
