cmake_minimum_required (VERSION 2.6)
project (xstl_tests)
set (xstl_tests_VERSION_MAJOR 1)
set (xstl_tests_VERSION_MINOR 0)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fPIC  -Wno-write-strings")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC  -Wno-write-strings")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out/bin)

if(NOT CMAKE_BUILD_TYPE)
	message(WARNING "cmake was execute without specify build type, run cmake -DCMAKE_BUILD_TYPE=Release for release build")
	set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -DDEBUG")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -DDEBUG")
endif()

if(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

add_executable(xstl_tests
     test_md5.cpp
     test_string.cpp
     test_counter.cpp
     test_osRandom.cpp
     test_stringStream.cpp
     sampleProtocol.cpp
     test_endian.cpp
     test_setArray.cpp
     test_threadClasses.cpp
     test_event.cpp
     test_pmac.cpp
     test_prf.cpp
     test_sha1.cpp
     test_alignment.cpp
     test_hash.cpp
     test_random.cpp
     test_smartptr.cpp
     test_array.cpp
     test_hmac_md5.cpp
     test_rle.cpp
     test_socket.cpp
     test_callback.cpp
     test_hmac_sha1.cpp
     tests.cpp
     test_stream.cpp)

set(ENV{XSTL_PATH} ../)

include_directories($ENV{XSTL_PATH}/Include)
include_directories($ENV{ELF_PATH}/Include)
include_directories($ENV{DISMOUNT_PATH}/Include)

find_package (Threads)
target_link_libraries(xstl_tests ${CMAKE_THREAD_LIBS_INIT})

# Try to find CMake single library
	find_library(LIBXSTLCM xStlcm HINTS $ENV{XSTL_PATH}/out/lib)
	target_link_libraries(xstl_tests ${LIBXSTLCM})
if(NOT LIBXSTLCM)
	find_library(LIBXSTL xstl HINTS $ENV{XSTL_PATH}/out/lib)
	find_library(LIBXSTL_DATA xstl_data HINTS $ENV{XSTL_PATH}/out/lib)
	find_library(LIBXSTL_EXCEPT xstl_except HINTS $ENV{XSTL_PATH}/out/lib)
	find_library(LIBXSTL_OS xstl_os HINTS $ENV{XSTL_PATH}/out/lib)
	find_library(LIBXSTL_PARSER xstl_parser HINTS $ENV{XSTL_PATH}/out/lib)
	find_library(LIBXSTL_STREAM xstl_stream HINTS $ENV{XSTL_PATH}/out/lib)
	find_library(LIBXSTL_UNIX xstl_unix HINTS $ENV{XSTL_PATH}/out/lib)

	target_link_libraries(xstl_tests ${LIBXSTL} ${LIBXSTL_DATA} ${LIBXSTL_EXCEPT}
	                              ${LIBXSTL_OS} ${LIBXSTL_PARSER} ${LIBXSTL_STREAM}
	                              ${LIBXSTL_UNIX})
endif()


if (UNIX)
	add_definitions(-DLINUX)
endif()
if (WIN32)
	add_definitions(-DWIN32)
endif()

add_definitions(-DXSTL_UNICODE)
add_definitions(-D_UNICODE)

target_compile_features(xstl_tests PRIVATE cxx_auto_type)
target_compile_features(xstl_tests PRIVATE cxx_range_for)
