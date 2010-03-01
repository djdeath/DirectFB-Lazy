engine_LTLIBRARIES = libdirectfb_lazy.la
enginedir = $(libdir)/@DIRECTFB_MODULEDIR@/gfxdrivers

libdirectfb_lazy_la_SOURCES = \
	lazy_gfxdriver.c \
	lazy_gfxdriver.h \
	lazy_osd.c \
	lazy_osd.h \
	lazy_osd_pool.c \
	lazy_osd_pool.h \
	lazy_passthrough.c \
	lazy_passthrough.h \
	lazy_passthrough_internal.h \
	lazy_screen.c \
	lazy_screen.h
libdirectfb_lazy_la_CFLAGS = @DIRECTFB_CFLAGS@
libdirectfb_lazy_la_LIBADD =
libdirectfb_lazy_la_LDFLAGS = @DIRECTFB_LIBS@

#  if you write a self-test script named `chk', uncomment the
#  following and add `chk' to the EXTRA_DIST list
#TESTS=chk
