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

#include "fimglib.h"

#define DEBUG_PRINT   printf

#define SEC_G2D_DEV_NAME             "/dev/fimg2d"
#define G2D_POLL_TIME 1000

static bool isFimglibInit = false;
static pthread_mutex_t instanceLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t sharedLock;
static int             g2dFd;

static unsigned char  *g2dVirtualAddress;
static unsigned int    g2dSize;
static unsigned char  *g2dDestinationVirtualAddress;
static unsigned int    g2dDestinationSize;
static struct pollfd   g2dPollFd;
static unsigned char  *sourceVirtualAddress;
static unsigned int    sourceSize;

static bool createContext()
{
    if (g2dFd != 0)
    {
        DEBUG_PRINT("%s(): m_g2dFd is non-zero\n", __func__);
        return false;
    }

#ifdef G2D_NONE_BLOCKING_MODE
    g2dFd = open(SEC_G2D_DEV_NAME, O_RDWR | O_NONBLOCK);
#else
    g2dFd = open(SEC_G2D_DEV_NAME, O_RDWR);
#endif
    if (g2dFd < 0)
    {
        DEBUG_PRINT("%s(): open(%s) fail(%s)\n", __func__, SEC_G2D_DEV_NAME, strerror(errno));
        g2dFd = 0;
        return false;
    }

    memset(&g2dPollFd, 0, sizeof(g2dPollFd));
    g2dPollFd.fd     = g2dFd;
    g2dPollFd.events = POLLOUT | POLLERR;

    return true;
}

static bool destroyContext(void)
{
    if (g2dVirtualAddress != NULL)
    {
        munmap(g2dVirtualAddress, g2dSize);
        g2dVirtualAddress = NULL;
        g2dSize = 0;
    }

    if (0 < g2dFd)
        close(g2dFd);

    g2dFd = 0;

    return true;
}

static bool executeIoctl(struct fimg2d_blit *cmd)
{
    if (ioctl(g2dFd, FIMG2D_BITBLT_BLIT, cmd) < 0)
        return false;

    return true;
}

inline static bool pollG2D(struct pollfd * events)
{

    int ret;

    ret = poll(events, 1, G2D_POLL_TIME);

    if (ret < 0)
    {
        DEBUG_PRINT("%s(): G2D poll failed\n", __func__);
        return false;
    }
    else if (ret == 0)
    {
        DEBUG_PRINT("%s(): no data in %d milliseconds\n", __func__, G2D_POLL_TIME);
        return false;
    }

    return true;
}

bool fimg_init()
{
    bool ret = false;
    pthread_mutex_lock(&instanceLock);

    // quick exit in case somebody else initialized it meanwhile
    if (isFimglibInit)
        return true;

    g2dFd = 0;
    sourceVirtualAddress = NULL;
    sourceSize = 0;
    g2dDestinationVirtualAddress = NULL;
    g2dVirtualAddress = NULL;
    g2dSize = 0;
    g2dDestinationSize = 0;
    memset(&(g2dPollFd), 0, sizeof(struct pollfd));

    //neagix: TODO: debug this
    pthread_mutexattr_t psharedm;
    pthread_mutexattr_init(&psharedm);
    pthread_mutexattr_setpshared(&psharedm, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&sharedLock, &psharedm);

    // acquire lock during creation
    pthread_mutex_lock(&sharedLock);

    if (createContext() == false)
    {
        DEBUG_PRINT("%s::m_CreateG2D() fail \n", __func__);

        if (destroyContext() == false)
            DEBUG_PRINT("%s::m_DestroyG2D() fail \n", __func__);

    }
    else
    {
        ret = true;
        isFimglibInit = true;
    }

    pthread_mutex_unlock(&sharedLock);
    pthread_mutex_unlock(&instanceLock);
    return ret;
}

void fimg_deinit()
{
    pthread_mutex_lock(&instanceLock);

    pthread_mutex_destroy(&sharedLock);
    isFimglibInit = false;

    pthread_mutex_unlock(&instanceLock);

    DEBUG_PRINT("%s() complete\n", __func__);
}

int fimg_stretch_internal(struct fimg2d_blit *cmd)
{
    if (!isFimglibInit)
    {
        DEBUG_PRINT("%s: fimg not initialized\n", __func__);
        return false;
    }

    bool ret = false;
    pthread_mutex_lock(&sharedLock);

    if (executeIoctl(cmd))
    {
#ifdef G2D_NONE_BLOCKING_MODE
        if (pollG2D(&g2dPollFd) == false)
        {
            DEBUG_PRINT("%s(): failed\n", __func__);
            ret = false;
        }
        else
#endif

            ret = true;
    }

    pthread_mutex_unlock(&sharedLock);

    return ret;
}

bool fimg_sync(void)
{
    if (!isFimglibInit)
    {
        DEBUG_PRINT("%s: fimg not initialized\n", __func__);
        return false;
    }

    if (pollG2D(&g2dPollFd) == false)
    {
        DEBUG_PRINT("%s::m_PollG2D() fail\n", __func__);
        return false;
    }
    return true;
}
