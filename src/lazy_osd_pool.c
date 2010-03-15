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

#include <config.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <asm/types.h>

#include <direct/debug.h>
#include <direct/mem.h>

#include <core/gfxcard.h>
#include <core/surface_pool.h>

#include <gfx/convert.h>

#include <misc/conf.h>

#include "lazy_gfxdriver.h"
#include "lazy_osd_pool.h"

D_DEBUG_DOMAIN( OSD_Surfaces, "Lazy/Surfaces", "Surface Manager" );
D_DEBUG_DOMAIN( OSD_SurfLock, "Lazy/SurfLock", "Surface Lock Manager" );

/**********************************************************************************************************************/

static int
osdPoolDataSize( void )
{
        return sizeof(LazyOSDPoolData);
}

static int
osdPoolLocalDataSize( void )
{
        return sizeof(LazyOSDPoolLocalData);
}

static int
osdAllocationDataSize( void )
{
        return sizeof(LazyOSDAllocationData);
}

static DFBResult
osdInitPool( CoreDFB                    *core,
             CoreSurfacePool            *pool,
             void                       *pool_data,
             void                       *pool_local,
             void                       *system_data,
             CoreSurfacePoolDescription *ret_desc )
{
        LazyOSDPoolData       *data  = pool_data;
        LazyOSDPoolLocalData  *local = pool_local;
        LazyDriverData *ddrv  = dfb_gfxcard_get_driver_data();
        LazyDeviceData *ddev  = dfb_gfxcard_get_device_data();

        D_DEBUG_AT( OSD_Surfaces, "%s()\n", __FUNCTION__ );

        D_ASSERT( core != NULL );
        D_MAGIC_ASSERT( pool, CoreSurfacePool );
        D_ASSERT( data != NULL );
        D_ASSERT( local != NULL );
        D_ASSERT( ret_desc != NULL );

        ret_desc->caps     = CSPCAPS_PHYSICAL | CSPCAPS_VIRTUAL;
        ret_desc->types    = CSTF_LAYER | CSTF_WINDOW | CSTF_SHARED | CSTF_EXTERNAL;
        ret_desc->priority = CSPP_PREFERED;

        ret_desc->access[CSAID_CPU]    = CSAF_READ | CSAF_WRITE | CSAF_SHARED;
        ret_desc->access[CSAID_GPU]    = CSAF_READ | CSAF_WRITE | CSAF_SHARED;
        ret_desc->access[CSAID_LAYER0] = CSAF_READ | CSAF_WRITE | CSAF_SHARED;

        snprintf( ret_desc->name, DFB_SURFACE_POOL_DESC_NAME_LENGTH, "Lazy OSD Pool" );

        local->core = core;
        /* local->mem  = ddrv->fb[OSD0].mem; */
        /* local->phys = ddev->fix[OSD0].smem_start; */

        D_MAGIC_SET( data, LazyOSDPoolData );
        D_MAGIC_SET( local, LazyOSDPoolLocalData );

        return DFB_OK;
}

static DFBResult
osdJoinPool( CoreDFB         *core,
             CoreSurfacePool *pool,
             void            *pool_data,
             void            *pool_local,
             void            *system_data )
{
        LazyOSDPoolData       *data  = pool_data;
        LazyOSDPoolLocalData  *local = pool_local;
        LazyDriverData *ddrv  = dfb_gfxcard_get_driver_data();
        LazyDeviceData *ddev  = dfb_gfxcard_get_device_data();

        D_DEBUG_AT( OSD_Surfaces, "%s()\n", __FUNCTION__ );

        D_ASSERT( core != NULL );
        D_MAGIC_ASSERT( pool, CoreSurfacePool );
        D_MAGIC_ASSERT( data, LazyOSDPoolData );
        D_ASSERT( local != NULL );

        (void) data;

        local->core = core;
        /* local->mem  = ddrv->fb[OSD0].mem; */
        /* local->phys = ddev->fix[OSD0].smem_start; */

        D_MAGIC_SET( local, LazyOSDPoolLocalData );

        return DFB_OK;
}

static DFBResult
osdDestroyPool( CoreSurfacePool *pool,
                void            *pool_data,
                void            *pool_local )
{
        LazyOSDPoolData      *data  = pool_data;
        LazyOSDPoolLocalData *local = pool_local;

        D_DEBUG_AT( OSD_Surfaces, "%s()\n", __FUNCTION__ );

        D_MAGIC_ASSERT( pool, CoreSurfacePool );
        D_MAGIC_ASSERT( data, LazyOSDPoolData );
        D_MAGIC_ASSERT( local, LazyOSDPoolLocalData );

        D_MAGIC_CLEAR( data );
        D_MAGIC_CLEAR( local );

        return DFB_OK;
}

static DFBResult
osdLeavePool( CoreSurfacePool *pool,
              void            *pool_data,
              void            *pool_local )
{
        LazyOSDPoolData      *data  = pool_data;
        LazyOSDPoolLocalData *local = pool_local;

        D_DEBUG_AT( OSD_Surfaces, "%s()\n", __FUNCTION__ );

        D_MAGIC_ASSERT( pool, CoreSurfacePool );
        D_MAGIC_ASSERT( data, LazyOSDPoolData );
        D_MAGIC_ASSERT( local, LazyOSDPoolLocalData );

        (void) data;

        D_MAGIC_CLEAR( local );

        return DFB_OK;
}

static DFBResult
osdTestConfig( CoreSurfacePool         *pool,
               void                    *pool_data,
               void                    *pool_local,
               CoreSurfaceBuffer       *buffer,
               const CoreSurfaceConfig *config )
{
        CoreSurface      *surface;
        LazyOSDPoolData      *data  = pool_data;
        LazyOSDPoolLocalData *local = pool_local;

        D_DEBUG_AT( OSD_Surfaces, "%s( %p )\n", __FUNCTION__, buffer );

        D_MAGIC_ASSERT( pool, CoreSurfacePool );
        D_MAGIC_ASSERT( data, LazyOSDPoolData );
        D_MAGIC_ASSERT( local, LazyOSDPoolLocalData );
        D_MAGIC_ASSERT( buffer, CoreSurfaceBuffer );

        (void) data;
        (void) local;

        surface = buffer->surface;
        D_MAGIC_ASSERT( surface, CoreSurface );

        /* if ((surface->config.format != DSPF_ARGB) ||  */
        /*     surface->config.format != DSPF_ARGB) */
        /*         return DFB_UNSUPPORTED; */

        return DFB_OK;
}

static DFBResult
osdAllocateBuffer( CoreSurfacePool       *pool,
                   void                  *pool_data,
                   void                  *pool_local,
                   CoreSurfaceBuffer     *buffer,
                   CoreSurfaceAllocation *allocation,
                   void                  *alloc_data )
{
        CoreSurface           *surface;
        LazyOSDPoolData       *data  = pool_data;
        LazyOSDPoolLocalData  *local = pool_local;
        LazyOSDAllocationData *alloc = alloc_data;
        LazyDeviceData        *ddev  = dfb_gfxcard_get_device_data();

        D_DEBUG_AT (OSD_Surfaces, "%s( %p )\n", __FUNCTION__, buffer);

        D_MAGIC_ASSERT (pool, CoreSurfacePool);
        D_MAGIC_ASSERT (data, LazyOSDPoolData);
        D_MAGIC_ASSERT (local, LazyOSDPoolLocalData);
        D_MAGIC_ASSERT (buffer, CoreSurfaceBuffer);

        (void) data;
        (void) local;

        surface = buffer->surface;
        D_MAGIC_ASSERT (surface, CoreSurface);

        if (!lazy_operation_addbuffer (surface->config.size.w,
                                       surface->config.size.h,
                                       DFB_BYTES_PER_PIXEL (surface->config.format),
                                       &alloc->buffer_id))
        {
                D_ERROR ("Cannot add buffer\n");

                return DFB_IO;
        }

        alloc->rowstride = DFB_BYTES_PER_PIXEL (surface->config.format) *
                surface->config.size.w;
        alloc->size = DFB_BYTES_PER_PIXEL (surface->config.format) *
                surface->config.size.w * surface->config.size.h;
        snprintf (alloc->filename, sizeof (alloc->filename),
                  "/tmp/%x", alloc->buffer_id);

        D_DEBUG_AT (OSD_Surfaces,
                    "  -> index %d, size %d => %s\n",
                    alloc->buffer_id,
                    alloc->size,
                    alloc->filename);

        if ((alloc->fd = open (alloc->filename,
                               O_CREAT | O_RDWR,
                               S_IRUSR | S_IWUSR | S_IRGRP |
                               S_IWGRP | S_IROTH | S_IWOTH)) == -1)
        {
                D_PERROR ("Cannot open %s\n", alloc->filename);

                return DFB_IO;
        }

        if ((alloc->ptr = mmap (NULL, alloc->size,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                alloc->fd, 0)) == MAP_FAILED)
        {
                D_PERROR ("Cannot mmap %s\n", alloc->filename);
                close (alloc->fd);

                return DFB_IO;
        }

        D_DEBUG_AT (OSD_Surfaces, "  -> ptr %p\n", alloc->ptr);

        allocation->size   = alloc->size;
        allocation->offset = 0;

        D_MAGIC_SET (alloc, LazyOSDAllocationData);

        return DFB_OK;
}

static DFBResult
osdDeallocateBuffer( CoreSurfacePool       *pool,
                     void                  *pool_data,
                     void                  *pool_local,
                     CoreSurfaceBuffer     *buffer,
                     CoreSurfaceAllocation *allocation,
                     void                  *alloc_data )
{
        LazyOSDPoolData       *data  = pool_data;
        LazyOSDAllocationData *alloc = alloc_data;

        D_DEBUG_AT( OSD_Surfaces, "%s( %p )\n", __FUNCTION__, buffer );

        D_MAGIC_ASSERT (pool, CoreSurfacePool);
        D_MAGIC_ASSERT (data, LazyOSDPoolData);
        D_MAGIC_ASSERT (buffer, CoreSurfaceBuffer);
        D_MAGIC_ASSERT (alloc, LazyOSDAllocationData);

        (void) data;

        if ((alloc->ptr != NULL) &&
            (alloc->ptr != MAP_FAILED))
        {
                munmap (alloc->ptr, alloc->size);
                alloc->ptr = NULL;
        }

        if (alloc->fd != -1)
        {
                close (alloc->fd);
                alloc->fd = -1;
        }

        lazy_operation_delbuffer (alloc->buffer_id);

        D_MAGIC_CLEAR (alloc);

        return DFB_OK;
}

static DFBResult
osdLock( CoreSurfacePool       *pool,
         void                  *pool_data,
         void                  *pool_local,
         CoreSurfaceAllocation *allocation,
         void                  *alloc_data,
         CoreSurfaceBufferLock *lock )
{
        LazyOSDPoolLocalData  *local = pool_local;
        LazyOSDAllocationData *alloc = alloc_data;
        LazyDeviceData *ddev  = dfb_gfxcard_get_device_data();

        D_MAGIC_ASSERT( pool, CoreSurfacePool );
        D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );
        D_MAGIC_ASSERT( alloc, LazyOSDAllocationData );
        D_MAGIC_ASSERT( lock, CoreSurfaceBufferLock );

        D_DEBUG_AT( OSD_SurfLock, "%s( %p )\n", __FUNCTION__, lock->buffer );

        allocation->size   = alloc->size;
        allocation->offset = 0;

        lock->pitch  = alloc->rowstride;
        lock->offset = 0;
        lock->addr   = alloc->ptr;
        lock->phys   = 0;

        D_DEBUG_AT( OSD_SurfLock, "  -> pitch %d, addr %p, phys 0x%08lx\n",
                    lock->pitch, lock->addr, lock->phys );

        return DFB_OK;
}

static DFBResult
osdUnlock( CoreSurfacePool       *pool,
           void                  *pool_data,
           void                  *pool_local,
           CoreSurfaceAllocation *allocation,
           void                  *alloc_data,
           CoreSurfaceBufferLock *lock )
{
        LazyOSDAllocationData *alloc = alloc_data;

        D_MAGIC_ASSERT( pool, CoreSurfacePool );
        D_MAGIC_ASSERT( allocation, CoreSurfaceAllocation );
        D_MAGIC_ASSERT( alloc, LazyOSDAllocationData );
        D_MAGIC_ASSERT( lock, CoreSurfaceBufferLock );

        D_DEBUG_AT( OSD_SurfLock, "%s( %p )\n", __FUNCTION__, lock->buffer );

        (void) alloc;

        return DFB_OK;
}

const SurfacePoolFuncs lazyOSDSurfacePoolFuncs = {
        .PoolDataSize       = osdPoolDataSize,
        .PoolLocalDataSize  = osdPoolLocalDataSize,
        .AllocationDataSize = osdAllocationDataSize,

        .InitPool           = osdInitPool,
        .JoinPool           = osdJoinPool,
        .DestroyPool        = osdDestroyPool,
        .LeavePool          = osdLeavePool,

        .TestConfig         = osdTestConfig,
        .AllocateBuffer     = osdAllocateBuffer,
        .DeallocateBuffer   = osdDeallocateBuffer,

        .Lock               = osdLock,
        .Unlock             = osdUnlock,
};
