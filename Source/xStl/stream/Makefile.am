INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl_stream.la

libxstl_stream_la_SOURCES = basicIO.cpp cacheStream.cpp filterStream.cpp lzw.cpp marginStringerStream.cpp memoryStream.cpp  \
                     rle.cpp socketAddr.cpp socketStream.cpp traceStream.cpp bitStream.cpp fileStream.cpp \
                     ioStream.cpp memoryAccesserStream.cpp socketException.cpp stringerStream.cpp \
                     endianFilterStream.cpp

libxstl_stream_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_stream_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_stream_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_stream_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif
