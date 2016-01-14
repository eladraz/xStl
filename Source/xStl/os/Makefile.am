INCLUDES = -I $(top_srcdir)/Include

if DEBUG
DBGFLAGS = -ggdb -DDEBUG
else
DBGFLAGS = -g
endif

lib_LTLIBRARIES = libxstl_os.la

# Need to add filename.cpp
libxstl_os_la_SOURCES = lockable.cpp  Makefile.am  osrand.cpp threadedClass.cpp  fragmentsDescriptor.cpp  lock.cpp  \
                        os.cpp   streamMemoryAccesser.cpp  thread.cpp  threadUnsafeMemoryAccesser.cpp  virtualMemoryAccesser.cpp
libxstl_os_la_CFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)
libxstl_os_la_CPPFLAGS = $(CFLAGS_XSTL_COMMON) $(DBGFLAGS) $(AM_CFLAGS)

if UNICODE
libxstl_os_la_CFLAGS+= -DXSTL_UNICODE -D_UNICODE
libxstl_os_la_CPPFLAGS+= -DXSTL_UNICODE -D_UNICODE
endif

