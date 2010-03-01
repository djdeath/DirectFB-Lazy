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

#ifndef __LAZY_GFXDRIVER_H__
#define __LAZY_GFXDRIVER_H__

#include <dfb_types.h>

#include <core/coretypes.h>
#include <core/layers.h>

typedef struct {
     int magic;

     CoreDFB *core;
     CoreSurfacePool *osd_pool;
} LazyDriverData;

typedef struct {
     int magic;
} LazyDeviceData;

#endif /* __LAZY_GFXDRIVER_H__ */
