INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl_digest.la

libxstl_digest_la_SOURCES = Crc16.cpp  Crc64.cpp  HashDigest.cpp  hmac.cpp   md5.cpp  sha1.cpp
libxstl_digest_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_digest_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_digest_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_digest_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif
