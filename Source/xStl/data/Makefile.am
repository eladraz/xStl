INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl_data.la

libxstl_data_la_SOURCES = Alignment.cpp  char.cpp  counter.cpp  datastream.cpp  endian.cpp  hash.cpp queueFifo.cpp  \
                     serializedObject.cpp  setArray.cpp  smartptr.cpp  string.cpp  wildcardMatcher.cpp
libxstl_data_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_data_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_data_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_data_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif
