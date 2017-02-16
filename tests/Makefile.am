INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

bin_PROGRAMS = xstl_tests

# Need to add test_filename.cpp
xstl_tests_SOURCES = test_compression.cpp \
                     test_md5.cpp \
                     test_string.cpp \
                     test_counter.cpp  \
                     test_osRandom.cpp \
                     test_stringStream.cpp \
                     sampleProtocol.cpp  \
                     test_endian.cpp       \
                     test_setArray.cpp  \
                     test_threadClasses.cpp \
                     test_event.cpp        \
                     test_pmac.cpp      \
                     test_prf.cpp       \
                     test_sha1.cpp      \
                     test_alignment.cpp  \
                     test_hash.cpp         \
                     test_random.cpp    \
                     test_smartptr.cpp \
                     test_array.cpp      \
                     test_hmac_md5.cpp     \
                     test_rle.cpp       \
                     test_socket.cpp    \
                     test_callback.cpp   \
                     test_hmac_sha1.cpp    \
                     tests.cpp          \
                     test_stream.cpp


xstl_tests_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
xstl_tests_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
xstl_tests_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
xstl_tests_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif

xstl_tests_LDADD = -L$(top_srcdir)/Source/xStl -lxstl \
                   -L$(top_srcdir)/Source/xStl/data -lxstl_data \
                   -L$(top_srcdir)/Source/xStl/except -lxstl_except \
                   -L$(top_srcdir)/Source/xStl/stream -lxstl_stream \
                   -L$(top_srcdir)/Source/xStl/os -lxstl_os \
                   -L$(top_srcdir)/Source/xStl/os/UnixOS -lxstl_unix \
                   -L$(top_srcdir)/Source/xStl/enc -lxstl_enc \
                   -L$(top_srcdir)/Source/xStl/enc/digest -lxstl_digest \
                   -L$(top_srcdir)/Source/xStl/enc/random -lxstl_random \
                   -L$(top_srcdir)/Source/xStl/enc/enc -lxstl_encryptions \
                   -L$(top_srcdir)/Source/xStl/utils -lxstl_utils


# TODO! Need to add support to libxstl_parser


