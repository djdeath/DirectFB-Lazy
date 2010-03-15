#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <pthread.h>

#define __LONG_TYPE_32__

#include "lazy_passthrough.h"
#include "lazy_passthrough_internal.h"

int socket_fd = -1;
pthread_mutex_t socket_mutex = PTHREAD_MUTEX_INITIALIZER;

#define SOCKET_LOCK(args...) do {                       \
                pthread_mutex_lock (&socket_mutex);     \
        } while (0)
#define SOCKET_UNLOCK(args...) do {                       \
                pthread_mutex_unlock (&socket_mutex);     \
        } while (0)

void
lazy_operation_init (void)
{
        struct sockaddr_in sv_addr;
	struct hostent *host;

        SOCKET_LOCK ();

        if (socket_fd != -1)
        {
                socket_fd = -1;
                close (socket_fd);
        }

        /* Resolv name */
	if (!(host = gethostbyname (LAZY_PASSTHROUGH_HOST)))
        {
                SOCKET_UNLOCK ();
		return;
        }

	memset (&sv_addr, 0, sizeof (struct sockaddr_in));

        sv_addr.sin_family = AF_INET;
	sv_addr.sin_addr = *((struct in_addr *) host->h_addr);
	sv_addr.sin_port = htons (LAZY_PASSTHROUGH_PORT);

        if ((socket_fd = socket (AF_INET, SOCK_STREAM, IPPROTO_IP)) >= 0)
	{
                if (connect (socket_fd,
                             (struct sockaddr *) &sv_addr,
                             sizeof (struct sockaddr)) < 0)
		{
			perror ("connect");
			close (socket_fd);
		}
	}
	else
		perror ("socket");

        SOCKET_UNLOCK ();
}

void
lazy_operation_fini (void)
{
        SOCKET_LOCK ();

        close (socket_fd);
        socket_fd = -1;

        SOCKET_UNLOCK ();
}

int
lazy_operation_addlayer (unsigned int layer_id, unsigned int buffer_id,
                         int width, int height,
                         int sx, int sy, int sw, int sh,
                         int dx, int dy, int dw, int dh)
{
        lazy_operation_addlayer_t op;
        lazy_operation_addlayer_res_t ret;

        memset (&op, 0, sizeof (op));
        op.operation = LAZY_OPERATION_ADD_LAYER;
        op.layer_id = layer_id;
        op.width = width;
        op.height = height;
        op.src.x = sx;
        op.src.y = sy;
        op.src.w = sw;
        op.src.h = sh;
        op.dst.x = dx;
        op.dst.y = dy;
        op.dst.w = dw;
        op.dst.h = dh;

        op.buffer_id = buffer_id;

        SOCKET_LOCK ();

        if (write (socket_fd, &op, sizeof (op)) != sizeof (op))
        {
                perror ("write");
                goto addlayer_fail;
        }

        if (read (socket_fd, &ret, sizeof (ret)) != sizeof (ret))
        {
                perror ("read");
                goto addlayer_fail;
        }

        if (ret.result != LAZY_OPERATION_RESULT_SUCCESS)
                goto addlayer_fail;

        SOCKET_UNLOCK ();

        return 1;

addlayer_fail:

        SOCKET_UNLOCK ();

        return 0;
}

int
lazy_operation_dellayer (unsigned int layer_id)
{
        lazy_operation_dellayer_t op;
        lazy_operation_dellayer_res_t ret;

        memset (&op, 0, sizeof (op));
        op.operation = LAZY_OPERATION_DEL_LAYER;
        op.layer_id = layer_id;

        SOCKET_LOCK ();

        if (write (socket_fd, &op, sizeof (op)) != sizeof (op))
        {
                perror ("write");
                goto addlayer_fail;
        }

        if (read (socket_fd, &ret, sizeof (ret)) != sizeof (ret))
        {
                perror ("read");
                goto addlayer_fail;
        }

        if (ret.result != LAZY_OPERATION_RESULT_SUCCESS)
                goto addlayer_fail;

        SOCKET_UNLOCK ();

        return 1;

addlayer_fail:

        SOCKET_UNLOCK ();

        return 0;
}

int
lazy_operation_fliplayer (unsigned int layer_id, unsigned int buffer_id)
{
        lazy_operation_fliplayer_t op;
        lazy_operation_fliplayer_res_t ret;

        memset (&op, 0, sizeof (op));
        op.operation = LAZY_OPERATION_FLIP_LAYER;
        op.layer_id = layer_id;
        op.buffer_id = buffer_id;

        SOCKET_LOCK ();

        if (write (socket_fd, &op, sizeof (op)) != sizeof (op))
        {
                perror ("write");
                goto addlayer_fail;
        }

        if (read (socket_fd, &ret, sizeof (ret)) != sizeof (ret))
        {
                perror ("read");
                goto addlayer_fail;
        }

        if (ret.result != LAZY_OPERATION_RESULT_SUCCESS)
                goto addlayer_fail;

        SOCKET_UNLOCK ();

        return 1;

addlayer_fail:

        SOCKET_UNLOCK ();

        return 0;
}

int
lazy_operation_addbuffer (int width, int height, int bpp,
                          unsigned int *buffer_id)
{
        lazy_operation_addbuffer_t op;
        lazy_operation_addbuffer_res_t ret;

        memset (&op, 0, sizeof (op));
        op.operation = LAZY_OPERATION_ADD_BUFFER;
        op.width = width;
        op.height = height;
        op.bpp = bpp;

        SOCKET_LOCK ();

        if (write (socket_fd, &op, sizeof (op)) != sizeof (op))
        {
                perror ("write");
                goto addlayer_fail;
        }

        if (read (socket_fd, &ret, sizeof (ret)) != sizeof (ret))
        {
                perror ("read");
                goto addlayer_fail;
        }

        if (ret.result != LAZY_OPERATION_RESULT_SUCCESS)
                goto addlayer_fail;

        SOCKET_UNLOCK ();

        *buffer_id = ret.buffer_id;

        return 1;

addlayer_fail:

        SOCKET_UNLOCK ();

        return 0;
}

int
lazy_operation_delbuffer (unsigned int buffer_id)
{
        lazy_operation_delbuffer_t op;
        lazy_operation_delbuffer_res_t ret;

        memset (&op, 0, sizeof (op));
        op.operation = LAZY_OPERATION_DEL_BUFFER;
        op.buffer_id = buffer_id;

        SOCKET_LOCK ();

        if (write (socket_fd, &op, sizeof (op)) != sizeof (op))
        {
                perror ("write");
                goto addlayer_fail;
        }

        if (read (socket_fd, &ret, sizeof (ret)) != sizeof (ret))
        {
                perror ("read");
                goto addlayer_fail;
        }

        if (ret.result != LAZY_OPERATION_RESULT_SUCCESS)
                goto addlayer_fail;

        SOCKET_UNLOCK ();

        return 1;

addlayer_fail:

        SOCKET_UNLOCK ();

        return 0;
}
