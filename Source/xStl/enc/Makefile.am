INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl_enc.la

libxstl_enc_la_SOURCES = digest.cpp  encryption.cpp  random.cpp  utils.cpp
libxstl_enc_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_enc_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_enc_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_enc_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif
