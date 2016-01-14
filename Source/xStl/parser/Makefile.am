INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl_parser.la

libxstl_parser_la_SOURCES = braces.cpp  except.cpp  parser.cpp  types.cpp  variable.cpp
libxstl_parser_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_parser_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_parser_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_parser_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif
