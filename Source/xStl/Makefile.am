INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl.la

libxstl_la_SOURCES = remoteAddress.cpp  types.cpp  xStlPrecompiled.cpp
libxstl_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif


libxstl_la_LIBADD = -L$(top_srcdir)/Source/xStl/data
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/enc
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/enc/digest
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/enc/enc
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/enc/random
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/except
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/os
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/os/UnixOS
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/parser
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/stream
libxstl_la_LIBADD+= -L$(top_srcdir)/Source/xStl/utils
libxstl_la_LIBADD+= -lxstl_except -lxstl_utils -lxstl_stream -lxstl_data -lxstl_parser -lxstl_unix -lxstl_os -lxstl_enc -lxstl_digest -lxstl_random -lxstl_encryptions

