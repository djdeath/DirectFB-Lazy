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

//#define DIRECT_ENABLE_DEBUG

#include <config.h>

#include <asm/types.h>

#include <stdio.h>
#include <sys/ioctl.h>

#include <directfb.h>
#include <directfb_util.h>

#include <core/layers.h>
#include <core/layers_internal.h>
#include <core/surface.h>
#include <core/surface_buffer.h>

#include <gfx/convert.h>

#include <direct/memcpy.h>
#include <direct/messages.h>

#include <misc/conf.h>

#include "lazy_gfxdriver.h"
#include "lazy_osd.h"
#include "lazy_osd_pool.h"
#include "lazy_passthrough.h"

#define D_OSDERROR(x...) do {} while (0)


D_DEBUG_DOMAIN( Lazy_OSD, "Lazy/Layers", "Layer manager" );

/**********************************************************************************************************************/

static int
osdLayerDataSize( void )
{
     return sizeof(LazyOSDLayerData);
}

static DFBResult
osdInitLayer( CoreLayer                  *layer,
              void                       *driver_data,
              void                       *layer_data,
              DFBDisplayLayerDescription *description,
              DFBDisplayLayerConfig      *config,
              DFBColorAdjustment         *adjustment )
{
     int                  ret;
     /* LazyDriverData   *ddrv = driver_data; */
     LazyOSDLayerData *dosd = layer_data;

     D_DEBUG_AT( Lazy_OSD, "%s()\n", __FUNCTION__ );

     /* set capabilities and type */
     description->caps = DLCAPS_SURFACE /* | DLCAPS_ALPHACHANNEL */ | DLCAPS_OPACITY | DLCAPS_SCREEN_POSITION /* | */
                         /* DLCAPS_SRC_COLORKEY */;
     description->type = DLTF_GRAPHICS;

     /* set name */
     snprintf( description->name, DFB_DISPLAY_LAYER_DESC_NAME_LENGTH, "TI Lazy OSD Layer" );

     /* fill out the default configuration */
     config->flags       = DLCONF_WIDTH       | DLCONF_HEIGHT |
                           DLCONF_PIXELFORMAT /* | DLCONF_BUFFERMODE */ | DLCONF_OPTIONS;
     config->width       = dfb_config->mode.width;
     config->height      = dfb_config->mode.height;
     config->pixelformat = DSPF_ARGB;
     /* config->buffermode  = DLBM_BACKVIDEO; */
     config->options     = DLOP_NONE;

     return DFB_OK;
}

static DFBResult
osdTestRegion( CoreLayer                  *layer,
               void                       *driver_data,
               void                       *layer_data,
               CoreLayerRegionConfig      *config,
               CoreLayerRegionConfigFlags *failed )
{
     CoreLayerRegionConfigFlags fail = 0;

     D_DEBUG_AT( Lazy_OSD, "%s()\n", __FUNCTION__ );

     if (config->options & ~LAZY_OSD_SUPPORTED_OPTIONS)
          fail |= CLRCF_OPTIONS;

     switch (config->format) {
          case DSPF_ARGB:
               break;

          default:
               fail |= CLRCF_FORMAT;
     }

     if (failed)
          *failed = fail;

     if (fail)
          return DFB_UNSUPPORTED;

     return DFB_OK;
}

static DFBResult
osdSetRegion( CoreLayer                  *layer,
              void                       *driver_data,
              void                       *layer_data,
              void                       *region_data,
              CoreLayerRegionConfig      *config,
              CoreLayerRegionConfigFlags  updated,
              CoreSurface                *surface,
              CorePalette                *palette,
              CoreSurfaceBufferLock      *lock )
{
     int                  ret;
     /* LazyDriverData   *ddrv = driver_data; */
     /* LazyDeviceData   *ddev = ddrv->ddev; */
     LazyOSDLayerData *dosd = layer_data;
     LazyOSDAllocationData *alloc = lock->allocation->data;

     D_DEBUG_AT( Lazy_OSD, "%s()\n", __FUNCTION__ );

     /* D_ASSERT( ddrv != NULL ); */
     /* D_ASSERT( ddev != NULL ); */
     D_ASSERT( dosd != NULL );
     D_MAGIC_ASSERT( alloc, LazyOSDAllocationData );

     ret = lazy_operation_addlayer (layer->shared->layer_id,
                                    alloc->buffer_index,
                                    layer->shared->default_config.width,
                                    layer->shared->default_config.height,
                                    config->dest.x, config->dest.y,
                                    config->dest.w, config->dest.h,
                                    config->dest.x, config->dest.y,
                                    config->dest.w, config->dest.h);
     if (ret != 1)
             return DFB_IO;

     return DFB_OK;
}

static DFBResult
osdRemoveRegion( CoreLayer *layer,
                 void      *driver_data,
                 void      *layer_data,
                 void      *region_data )
{
     int                  ret;
     /* LazyDriverData   *ddrv = driver_data; */
     LazyOSDLayerData *dosd = layer_data;

     D_DEBUG_AT( Lazy_OSD, "%s()\n", __FUNCTION__ );

     /* D_ASSERT( ddrv != NULL ); */

     ret = lazy_operation_dellayer (layer->shared->layer_id);
     if (ret != 1)
             return DFB_IO;

     return DFB_OK;
}

static DFBResult
osdFlipRegion( CoreLayer             *layer,
               void                  *driver_data,
               void                  *layer_data,
               void                  *region_data,
               CoreSurface           *surface,
               DFBSurfaceFlipFlags    flags,
               CoreSurfaceBufferLock *lock )
{
     int                  ret;
     CoreSurfaceBuffer   *buffer;
     /* LazyDriverData   *ddrv = driver_data; */
     /* LazyDeviceData   *ddev = ddrv->ddev; */
     LazyOSDLayerData *dosd = layer_data;
     LazyOSDAllocationData *alloc = lock->allocation->data;

     D_DEBUG_AT( Lazy_OSD, "%s()\n", __FUNCTION__ );

     D_ASSERT( surface != NULL );
     D_ASSERT( lock != NULL );
     /* D_ASSERT( ddrv != NULL ); */
     /* D_ASSERT( ddev != NULL ); */
     D_MAGIC_ASSERT( alloc, LazyOSDAllocationData );

     ret = lazy_operation_fliplayer (layer->shared->layer_id,
                                     alloc->buffer_index);
     if (ret != 1)
             return DFB_IO;

     dfb_surface_flip( surface, false );

     return DFB_OK;
}

static DFBResult
osdUpdateRegion( CoreLayer             *layer,
                 void                  *driver_data,
                 void                  *layer_data,
                 void                  *region_data,
                 CoreSurface           *surface,
                 const DFBRegion       *update,
                 CoreSurfaceBufferLock *lock )
{
     int                  ret;
     CoreSurfaceBuffer   *buffer;
     /* LazyDriverData   *ddrv = driver_data; */
     /* LazyDeviceData   *ddev = ddrv->ddev; */
     LazyOSDLayerData *dosd = layer_data;
     LazyOSDAllocationData *alloc = lock->allocation->data;

     D_DEBUG_AT( Lazy_OSD, "%s()\n", __FUNCTION__ );

     D_ASSERT( surface != NULL );
     D_ASSERT( lock != NULL );
     /* D_ASSERT( ddrv != NULL ); */
     /* D_ASSERT( ddev != NULL ); */
     D_MAGIC_ASSERT( alloc, LazyOSDAllocationData );

     ret = lazy_operation_fliplayer (layer->shared->layer_id,
                                     alloc->buffer_index);
     if (ret != 1)
             return DFB_IO;

     return DFB_OK;
}

DisplayLayerFuncs lazyOSDLayerFuncs = {
     .LayerDataSize = osdLayerDataSize,
     .InitLayer     = osdInitLayer,

     .TestRegion    = osdTestRegion,
     .SetRegion     = osdSetRegion,
     .RemoveRegion  = osdRemoveRegion,
     .FlipRegion    = osdFlipRegion,
     .UpdateRegion  = osdUpdateRegion,
};
