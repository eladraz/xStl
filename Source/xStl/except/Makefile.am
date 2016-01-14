INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl_except.la

libxstl_except_la_SOURCES = assert.cpp  exception.cpp  trace.cpp  traceStack.cpp
libxstl_except_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_except_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_except_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_except_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif
