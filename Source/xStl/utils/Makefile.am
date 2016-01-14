INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl_utils.la

libxstl_utils_la_SOURCES = dumpMemory.cpp  TimeoutMonitor.cpp
libxstl_utils_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_utils_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_utils_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_utils_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif
