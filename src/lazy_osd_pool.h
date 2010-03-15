/*
  (c) Copyright 2009  The world wide DirectFB Open Source Community (directfb.org)

  All rights reserved.

  Written by Lionel Landwerlin <llandwerlin@gmail.com>.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
  Boston, MA 02111-1307, USA.
*/

#ifndef __LAZY_OSD_POOL_H__
#define __LAZY_OSD_POOL_H__

#include <core/core.h>
#include <core/surface_pool.h>

typedef struct {
        int magic;
        /* int alloc_index; */
        /* int alloc_count; */
} LazyOSDPoolData;

typedef struct {
        int      magic;

        CoreDFB *core;
} LazyOSDPoolLocalData;

typedef struct {
        int   magic;

        unsigned int buffer_id;

        int rowstride;
        int size;

        int   fd;
        void *ptr;
        char  filename[256];
} LazyOSDAllocationData;

extern const SurfacePoolFuncs lazyOSDSurfacePoolFuncs;

#endif /* __LAZY_OSD_POOL_H__ */
