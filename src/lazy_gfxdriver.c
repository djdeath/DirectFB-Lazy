/*
   (c) Copyright 2009  The DirectFB Organization (directfb.org)

   All rights reserved.

   Written by Lionel Landwerlin <llandwerlin@gmail.com>

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

#include <config.h>

#include <dfb_types.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <malloc.h>

#include <directfb.h>

#include <core/coredefs.h>
#include <core/coretypes.h>
#include <core/screens.h>
#include <core/system.h>

#include <core/state.h>
#include <core/gfxcard.h>
#include <core/surface_buffer.h>

#include <gfx/convert.h>
#include <gfx/util.h>
#include <misc/conf.h>
#include <misc/util.h>

#include <core/graphics_driver.h>

DFB_GRAPHICS_DRIVER( lazy )

#include "lazy_gfxdriver.h"
#include "lazy_osd.h"
#include "lazy_osd_pool.h"
#include "lazy_passthrough.h"
#include "lazy_screen.h"

D_DEBUG_DOMAIN( lazy, "Lazy/GfxDriver", "Lazy GFX driver" );


#define LAZY_SUPPORTED_DRAWINGFLAGS (DSDRAW_BLEND | DSDRAW_DST_COLORKEY)

#define LAZY_SUPPORTED_DRAWINGFUNCTIONS (DFXL_FILLRECTANGLE | \
                                         DFXL_DRAWRECTANGLE | \
                                         DFXL_DRAWLINE |      \
                                         DFXL_FILLTRIANGLE |  \
                                         DFXL_BLIT |          \
                                         DFXL_STRETCHBLIT |   \
                                         DFXL_TEXTRIANGLES |  \
                                         DFXL_BLIT2)

#define LAZY_SUPPORTED_BLITTINGFLAGS (DSBLIT_BLEND_ALPHACHANNEL |       \
                                      DSBLIT_BLEND_COLORALPHA |         \
                                      DSBLIT_COLORIZE |                 \
                                      DSBLIT_SRC_COLORKEY |             \
                                      DSBLIT_DST_COLORKEY |             \
                                      DSBLIT_SRC_PREMULTIPLY |          \
                                      DSBLIT_DST_PREMULTIPLY |          \
                                      DSBLIT_DEMULTIPLY |               \
                                      DSBLIT_SRC_PREMULTCOLOR |         \
                                      DSBLIT_XOR |                      \
                                      DSBLIT_ROTATE90 |                 \
                                      DSBLIT_ROTATE180 |                \
                                      DSBLIT_ROTATE270 |                \
                                      DSBLIT_COLORKEY_PROTECT |         \
                                      DSBLIT_SRC_MASK_ALPHA |           \
                                      DSBLIT_SOURCE2 |                  \
                                      DSBLIT_FLIP_HORIZONTAL |          \
                                      DSBLIT_FLIP_VERTICAL)

#define LAZY_SUPPORTED_BLITTINGFUNCTIONS  (DFXL_FILLRECTANGLE | \
                                           DFXL_DRAWRECTANGLE | \
                                           DFXL_DRAWLINE |      \
                                           DFXL_FILLTRIANGLE |  \
                                           DFXL_BLIT |          \
                                           DFXL_STRETCHBLIT |   \
                                           DFXL_TEXTRIANGLES |  \
                                           DFXL_BLIT2)

static void
lazyCheckState(void *drv, void *dev,
               CardState *state, DFBAccelerationMask accel )
{
     /* switch (state->destination->config.format) { */
     /*      case DSPF_RGB16: */
     /*      case DSPF_RGB24: */
     /*      case DSPF_RGB32: */
     /*           break; */
     /*      default: */
     /*           return; */
     /* } */

     /* if (!(accel & ~LAZY_SUPPORTED_DRAWINGFUNCTIONS) && */
     /*     !(state->drawingflags & ~LAZY_SUPPORTED_DRAWINGFLAGS)) */
     /*      state->accel |= LAZY_SUPPORTED_DRAWINGFUNCTIONS; */


     /* if (!(accel & ~LAZY_SUPPORTED_BLITTINGFUNCTIONS) && */
     /*     !(state->blittingflags & ~LAZY_SUPPORTED_BLITTINGFLAGS)) { */
     /*      if (state->source->config.format == state->destination->config.format) */
     /*           state->accel |= LAZY_SUPPORTED_BLITTINGFUNCTIONS; */
     /* } */
}

static void
lazySetState( void *drv, void *dev,
              GraphicsDeviceFuncs *funcs,
              CardState *state, DFBAccelerationMask accel )
{
     /* LazyDriverData *lazydrv = (LazyDriverData *) drv; */
     /* LazyDeviceData *lazydev = (LazyDeviceData *) dev; */

     /* if (state->modified & SMF_SOURCE && state->source ) */
     /*      lazydev->smf_source = 0; */

     /* if (state->modified & SMF_DESTINATION) */
     /*      lazydev->smf_destination = lazydev->smf_color = 0; */

     /* if (state->modified & SMF_COLOR) */
     /*      lazydev->smf_color = 0; */

     /* if (state->modified & SMF_CLIP) */
     /*      lazydev->smf_clip = 0; */

     /* lazy_set_destination( lazydrv, lazydev, state); */

     /* switch (accel) { */
     /*      case DFXL_FILLRECTANGLE: */
     /*           lazy_set_color( lazydrv, lazydev, state ); */
     /*           state->set |= DFXL_FILLRECTANGLE; */
     /*           break; */
     /*      case DFXL_DRAWLINE: */
     /*           lazy_set_color( lazydrv, lazydev, state ); */
     /*           state->set |= DFXL_DRAWLINE ; */
     /*           break; */

     /*      case DFXL_BLIT: */
     /*           lazy_set_src( lazydrv, lazydev, state ); */
     /*           state->set |= DFXL_BLIT; */
     /*           break; */
     /*      default: */
     /*           D_ERROR( "unexpected drawing/blitting function" ); */
     /*           break; */

     /* } */

     /* if (state->modified & SMF_CLIP) */
     /*      lazy_set_clip( lazydrv, lazydev, &state->clip); */

     /* state->modified = 0; */
}

/* static void */
/* lazyFlushTextureCache(void *drv, void *dev) */
/* { */
/*      LazyDeviceData *lazydev = (LazyDeviceData *) dev; */

/*      lazydev->srcaddr = lazydev->destaddr = 0; */
/*      lazydev->srcpitch = lazydev->destpitch = 0; */
/*      lazydev->fb_store = false; */

/* } */

static DFBResult
lazyEngineSync(void *drv, void *dev)
{

     return DFB_OK;
}

static bool
lazyFillRectangle( void *drv, void *dev, DFBRectangle *rect )
{
     /* LazyDriverData *lazydrv = (LazyDriverData *) drv; */
     /* LazyDeviceData *lazydev = (LazyDeviceData *) dev; */
     /* struct lazy_fill fill; */
     D_DEBUG_AT(lazy,"%s:enter\n",__FUNCTION__);

     /* fill.dx = rect->x; */
     /* fill.dy = rect->y; */
     /* fill.width = rect->w; */
     /* fill.height = rect->h; */
     /* fill.color = lazydev->fill_color; */
     /* ioctl(lazydrv->dfb_fbdev->fd,FBIO_LAZY_FILL, &fill); */
     /* D_DEBUG_AT(lazy,"%s:exit\n",__FUNCTION__); */
     return true;
}

static bool
lazyDrawLine( void *drv, void *dev, DFBRegion *line )
{
     /* LazyDriverData *lazydrv = (LazyDriverData *) drv; */
     /* LazyDeviceData *lazydev = (LazyDeviceData *) dev; */

     /* struct lazy_line drawline; */
     D_DEBUG_AT(lazy,"%s:enter\n",__FUNCTION__);
     /* drawline.flags = 0; */
     /* drawline.x1 = line->x1; */
     /* drawline.x2 = line->x2; */
     /* drawline.y1 = line->y1; */
     /* drawline.y2 = line->y2; */
     /* drawline.fgcolor = lazydev->fill_color; */
     /* drawline.bgcolor = 0; */
     /* drawline.pattern = 0; */

     /* ioctl(lazydrv->dfb_fbdev->fd,FBIO_LAZY_LINE, &drawline); */
     /* D_DEBUG_AT(lazy,"%s:exit\n",__FUNCTION__); */
     return true;
}


static bool
lazyBlit( void *drv, void *dev, DFBRectangle *rect, int dx, int dy )
{

     /* LazyDriverData *lazydrv = (LazyDriverData *) drv; */
     /* LazyDeviceData *lazydev = (LazyDeviceData *) dev; */
     /* struct fb_image image; */

     D_DEBUG_AT(lazy,"%s:enter\n",__FUNCTION__);

     /* if (!(lazydev->clip.x1 <= dx) || !(lazydev->clip.y1 <= dy) || */
     /*     !( lazydev->clip.x2 >= (dx + rect->w - 1) ) || !( lazydev->clip.y2 >= (dy + rect->h - 1) )) { */
     /*      D_ERROR("the blit region is not vaild\n"); */
     /*      return false; */
     /* } */

     /* image.dx = lazydev->destaddr + dx; */
     /* image.dy = dy; */
     /* image.width = rect->w; */
     /* image.height = rect->h; */
     /* image.depth = lazydev->pixeldepth; */
     /* if ( lazydev->fb_store == true ) */
     /*      image.data = (void*)lazydev->fb_addr + lazydev->srcaddr + DFB_BYTES_PER_LINE( lazydev->pixelformat,rect->x ) + (rect->y * lazydev->srcpitch ); */
     /* else */
     /*      image.data = (void*)lazydev->srcaddr + DFB_BYTES_PER_LINE( lazydev->pixelformat, rect->x ) + (rect->y * lazydev->srcpitch ); */

     /* ioctl(lazydrv->dfb_fbdev->fd,FBIO_LAZY_BLIT,&image); */

     /* D_DEBUG_AT(lazy,"%s:exit\n",__FUNCTION__); */

     return true;
}

static int
driver_probe( CoreGraphicsDevice *device )
{
     D_DEBUG_AT (lazy, "%s: Enter\n", __FUNCTION__);

     if (dfb_system_type() == CORE_NULL)
          return 1;

     D_DEBUG_AT (lazy, "%s: Exit accel=%i\n", __FUNCTION__,
                 dfb_gfxcard_get_accelerator( device ));

     return 0;
}

static void
driver_get_info( CoreGraphicsDevice *device,
                 GraphicsDriverInfo *info )
{
     /* fill driver info structure */
     snprintf( info->name,
               DFB_GRAPHICS_DRIVER_INFO_NAME_LENGTH,
               "Lazy Driver" );

     snprintf( info->vendor,
               DFB_GRAPHICS_DRIVER_INFO_VENDOR_LENGTH,
               "DirectFB" );

     snprintf( info->url,
               DFB_GRAPHICS_DRIVER_INFO_URL_LENGTH,
               "http://directfb.org" );

     snprintf( info->license,
               DFB_GRAPHICS_DRIVER_INFO_LICENSE_LENGTH,
               "LGPL" );

     info->version.major = 0;
     info->version.minor = 1;

     info->driver_data_size = sizeof (LazyDriverData);
     info->device_data_size = sizeof (LazyDeviceData);
}

static DFBResult
driver_init_driver( CoreGraphicsDevice  *device,
                    GraphicsDeviceFuncs *funcs,
                    void                *driver_data,
                    void                *device_data,
                    CoreDFB             *core )
{
     CoreScreen *screen;
     CoreLayer *primary;
     LazyDriverData *lazydrv = (LazyDriverData*) driver_data;
     LazyDeviceData *lazydev = (LazyDeviceData*) device_data;

     D_DEBUG_AT (lazy, "%s: Enter\n", __FUNCTION__);

     funcs->CheckState    = lazyCheckState;
     funcs->SetState      = lazySetState;
     funcs->EngineSync    = lazyEngineSync;

     funcs->FillRectangle = lazyFillRectangle;
     funcs->DrawLine          = lazyDrawLine;
     funcs->Blit          = lazyBlit;

     screen = dfb_screens_register (device,
                                    lazydrv,
                                    &lazyScreenFuncs);
     primary = dfb_layers_register (screen, lazydrv,
                                    &lazyOSDLayerFuncs);

     dfb_surface_pool_initialize (core,
                                  &lazyOSDSurfacePoolFuncs,
                                  &lazydrv->osd_pool);

     lazydrv->core = core;

     D_MAGIC_SET( lazydrv, LazyDriverData );

     D_DEBUG_AT (lazy, "%s: Exit\n", __FUNCTION__);

     return DFB_OK;
}


static DFBResult
driver_init_device( CoreGraphicsDevice *device,
                    GraphicsDeviceInfo *device_info,
                    void               *driver_data,
                    void               *device_data )
{

     LazyDriverData *lazydrv = (LazyDriverData*) driver_data;
     LazyDeviceData *lazydev = (LazyDeviceData*) device_data;
     bool master = dfb_core_is_master (lazydrv->core);

     D_DEBUG_AT (lazy, "%s: Enter\n", __FUNCTION__);

     /* fill device info */
     snprintf( device_info->name,
               DFB_GRAPHICS_DEVICE_INFO_NAME_LENGTH, "lazy" );

     snprintf( device_info->vendor,
               DFB_GRAPHICS_DEVICE_INFO_VENDOR_LENGTH, "DirectFB" );

     device_info->caps.flags    = 0;
     device_info->caps.accel    = LAZY_SUPPORTED_DRAWINGFUNCTIONS |
                                  LAZY_SUPPORTED_BLITTINGFUNCTIONS;

     device_info->caps.drawing  = LAZY_SUPPORTED_DRAWINGFLAGS;
     device_info->caps.blitting = LAZY_SUPPORTED_BLITTINGFLAGS;

     device_info->limits.surface_byteoffset_alignment = 32 * 4;
     device_info->limits.surface_pixelpitch_alignment = 32;

     lazy_operation_init();

     if (!master)
     {
          dfb_surface_pool_join (lazydrv->core,
                                 lazydrv->osd_pool,
                                 &lazyOSDSurfacePoolFuncs);
     }


     D_MAGIC_SET( lazydev, LazyDeviceData );

     D_DEBUG_AT (lazy, "%s: Exit\n", __FUNCTION__);

     return DFB_OK;
}


static void
driver_close_device( CoreGraphicsDevice *device,
                     void               *driver_data,
                     void               *device_data )
{
     LazyDeviceData *lazydev = (LazyDeviceData*) device_data;

     D_DEBUG_AT (lazy, "%s: Enter\n", __FUNCTION__);

     lazy_operation_fini();

     D_MAGIC_CLEAR( lazydev );

     D_DEBUG_AT (lazy, "%s: Exit\n", __FUNCTION__);
}

static void
driver_close_driver( CoreGraphicsDevice *device,
                     void               *driver_data )
{

     LazyDriverData *lazydrv = (LazyDriverData*) driver_data;

     D_DEBUG_AT (lazy, "%s: Enter\n", __FUNCTION__);

     D_MAGIC_CLEAR( lazydrv );

     D_DEBUG_AT (lazy, "%s: Exit\n", __FUNCTION__);
}
