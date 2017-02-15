cmake_minimum_required (VERSION 2.6)
project (xStl)
set (xStl_VERSION_MAJOR 1)
set (xStl_VERSION_MINOR 0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC  -Wno-write-strings")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC  -Wno-write-strings")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

list(APPEND XSTL_LIB_FILES
	Source/xStl/types.cpp
	Source/xStl/remoteAddress.cpp
)

list(APPEND XSTL_LIB_FILES
	Source/xStl/data/Alignment.cpp
	Source/xStl/data/char.cpp
	Source/xStl/data/counter.cpp
	Source/xStl/data/datastream.cpp
	Source/xStl/data/endian.cpp
	Source/xStl/data/hash.cpp
	Source/xStl/data/queueFifo.cpp
	Source/xStl/data/serializedObject.cpp
	Source/xStl/data/setArray.cpp
	Source/xStl/data/smartptr.cpp
	Source/xStl/data/string.cpp
	Source/xStl/data/wildcardMatcher.cpp
)

list(APPEND XSTL_LIB_FILES
	Source/xStl/enc/digest.cpp
	Source/xStl/enc/encryption.cpp
	Source/xStl/enc/random.cpp
	Source/xStl/enc/utils.cpp
	Source/xStl/enc/digest/Crc16.cpp
	Source/xStl/enc/digest/Crc64.cpp
	Source/xStl/enc/digest/HashDigest.cpp
	Source/xStl/enc/digest/hmac.cpp
	Source/xStl/enc/digest/md5.cpp
	Source/xStl/enc/digest/sha1.cpp
	Source/xStl/enc/enc/rc4.cpp
	Source/xStl/enc/random/pmacRnd.cpp
	Source/xStl/enc/random/prf.cpp
	Source/xStl/enc/random/rc4rnd.cpp
)

list(APPEND XSTL_LIB_FILES
	Source/xStl/except/assert.cpp
	Source/xStl/except/exception.cpp
	Source/xStl/except/trace.cpp
	Source/xStl/except/traceStack.cpp
)

list(APPEND XSTL_LIB_FILES
	Source/xStl/os/lockable.cpp
	Source/xStl/os/osrand.cpp
	Source/xStl/os/threadedClass.cpp
	Source/xStl/os/fragmentsDescriptor.cpp
	Source/xStl/os/lock.cpp
	Source/xStl/os/os.cpp
	Source/xStl/os/streamMemoryAccesser.cpp
	Source/xStl/os/thread.cpp
	Source/xStl/os/threadUnsafeMemoryAccesser.cpp
	Source/xStl/os/virtualMemoryAccesser.cpp
)

list(APPEND XSTL_LIB_FILES
	Source/xStl/parser/braces.cpp
	Source/xStl/parser/except.cpp
	Source/xStl/parser/parser.cpp
	Source/xStl/parser/types.cpp
	Source/xStl/parser/variable.cpp
)

list(APPEND XSTL_LIB_FILES
	Source/xStl/stream/basicIO.cpp
	Source/xStl/stream/cacheStream.cpp
	Source/xStl/stream/filterStream.cpp
	Source/xStl/stream/marginStringerStream.cpp
	Source/xStl/stream/memoryStream.cpp
	Source/xStl/stream/rle.cpp
	Source/xStl/stream/socketAddr.cpp
	Source/xStl/stream/socketStream.cpp
	Source/xStl/stream/traceStream.cpp
	Source/xStl/stream/bitStream.cpp
	Source/xStl/stream/fileStream.cpp
	Source/xStl/stream/ioStream.cpp
	Source/xStl/stream/memoryAccesserStream.cpp
	Source/xStl/stream/socketException.cpp
	Source/xStl/stream/stringerStream.cpp
	Source/xStl/stream/endianFilterStream.cpp
)

list(APPEND XSTL_LIB_FILES
	Source/xStl/utils/dumpMemory.cpp
	Source/xStl/utils/TimeoutMonitor.cpp
)

if (UNIX)
	set(CMAKE_MACOSX_RPATH 1)
	add_definitions(-DLINUX)
	list(APPEND XSTL_LIB_FILES
		Source/xStl/os/UnixOS/unixFile.cpp
		Source/xStl/os/UnixOS/unixOSRand.cpp
		Source/xStl/os/UnixOS/unixThread.cpp
		Source/xStl/os/UnixOS/unixOS.cpp
		Source/xStl/os/UnixOS/unixMutex.cpp
		Source/xStl/os/UnixOS/unixEvent.cpp
	)
endif()
if (WIN32)
	add_definitions(-DWIN32)
	list(APPEND XSTL_LIB_FILES
		Source/xStl/WindowsOS/event.cpp
		Source/xStl/WindowsOS/mutex.cpp
		Source/xStl/WindowsOS/osExcept.cpp
		Source/xStl/WindowsOS/winFile.cpp
		Source/xStl/WindowsOS/winFilename.cpp
		Source/xStl/WindowsOS/winRandom.cpp
		Source/xStl/WindowsOS/winThread.cpp
		Source/xStl/WindowsOS/windowsOS.cpp
		Source/xStl/stream/pipeStream.cpp
	)
endif()

include_directories(Include)
add_library(xStlcm_static STATIC ${XSTL_LIB_FILES})
add_library(xStlcm SHARED ${XSTL_LIB_FILES})

add_definitions(-DXSTL_UNICODE)
add_definitions(-D_UNICODE)
