if HAVE_MAGICK

plugindir = $(libdir)/ferris/plugins/eagenerators

INCLUDES = -I$(top_builddir)/lib -I$(top_srcdir)/lib 						\
	-I$(srcdir)/..                                  \
	-I$(top_srcdir)/src 				\
	-I$(includedir)					\
	@MAGICK_CFLAGS@                                 \
	@CFLAGS@

plugin_LTLIBRARIES = libmagick.la 
libmagick_la_SOURCES = libmagick.cpp
libmagick_la_LDFLAGS = @GLIB_LIBS@       \
	@MAGICK_LIBS@                    \
	@LIBFERRIS_EAPLUGIN_LT_LINE@

endif
