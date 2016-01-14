INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl_unix.la

libxstl_unix_la_SOURCES = unixFile.cpp  unixMutex.cpp  unixOS.cpp  unixThread.cpp  unixOSRand.cpp  unixEvent.cpp
libxstl_unix_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_unix_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_unix_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_unix_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif
