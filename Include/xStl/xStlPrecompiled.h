/*
 * Copyright (c) 2008-2016, Integrity Project Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * Neither the name of the Integrity Project nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE
 */

#include "xStl/types.h"
#include "xStl/exceptions.h"
#include "xStl/operators.h"
#include "xStl/refCountObject.h"
#include "xStl/remoteAddress.h"

#ifdef PRECOMPILED_HEADERS
#include "xStl/data/alignment.h"
#include "xStl/data/autoReference.h"
#include "xStl/data/array.h"
#include "xStl/data/char.h"
#include "xStl/data/counter.h"
#include "xStl/data/string.h"
#include "xStl/data/datastream.h"
#include "xStl/data/dualElement.h"
#include "xStl/data/endian.h"
#include "xStl/data/hash.h"
#include "xStl/data/list.h"
//#include "xStl/data/messageQueue.h"
#include "xStl/data/queueFifo.h"
#include "xStl/data/sarray.h"
#include "xStl/data/serializedObject.h"
#include "xStl/data/setArray.h"
#include "xStl/data/smartptr.h"
#include "xStl/data/wildcardMatcher.h"
#include "xStl/utils/algorithm.h"
#include "xStl/utils/arguments.h"
#include "xStl/utils/callbacker.h"
#include "xStl/utils/dumpMemory.h"
#include "xStl/utils/genericCallbackerFunctor.h"
#include "xStl/utils/TimeoutMonitor.h"
#include "xStl/os/directoryFormatParser.h"
#include "xStl/os/event.h"
#include "xStl/os/file.h"
#include "xStl/os/fileException.h"
#include "xStl/os/filename.h"
#include "xStl/os/fragmentsDescriptor.h"
#include "xStl/os/lock.h"
#include "xStl/os/lockable.h"
#include "xStl/os/mutex.h"
#include "xStl/os/os.h"
#include "xStl/os/osdef.h"
#include "xStl/os/osExcept.h"
#include "xStl/os/serialPort.h"
#include "xStl/os/streamMemoryAccesser.h"
#include "xStl/os/thread.h"
#include "xStl/os/threadedClass.h"
#include "xStl/os/threadUnsafeMemoryAccesser.h"
#include "xStl/os/time.h"
#include "xStl/os/virtualMemoryAccesser.h"
#include "xStl/os/waitable.h"
#include "xStl/os/xstlLockable.h"
#include "xStl/enc/digest.h"
#include "xStl/enc/encryption.h"
#include "xStl/enc/random.h"
#include "xStl/enc/utils.h"
#include "xStl/enc/digest/Crc16.h"
#include "xStl/enc/digest/HashDigest.h"
#include "xStl/enc/digest/hmac.h"
#include "xStl/enc/digest/md5.h"
#include "xStl/enc/digest/sha1.h"
#include "xStl/enc/random/pmacRnd.h"
#include "xStl/enc/random/prf.h"
#include "xStl/enc/random/rc4rnd.h"
#include "xStl/enc/enc/rc4.h"
#include "xStl/except/assert.h"
#include "xStl/except/exception.h"
#include "xStl/except/trace.h"
#include "xStl/except/traceStack.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/bitStream.h"
#include "xStl/stream/cacheStream.h"
#include "xStl/stream/deserializer.h"
#include "xStl/stream/fileStream.h"
#include "xStl/stream/filterStream.h"
#include "xStl/stream/forkStream.h"
#include "xStl/stream/ioStream.h"
#include "xStl/stream/lzw.h"
#include "xStl/stream/marginStringerStream.h"
#include "xStl/stream/memoryAccesserStream.h"
#include "xStl/stream/pipeStream.h"
#include "xStl/stream/memoryStream.h"
#include "xStl/stream/rle.h"
#include "xStl/stream/serializedObject.h"
#include "xStl/stream/serialStream.h"
#include "xStl/stream/socketAddr.h"
#include "xStl/stream/socketException.h"
#include "xStl/stream/socketStream.h"
#include "xStl/stream/stringerStream.h"
#include "xStl/stream/traceStream.h"

#endif // PRECOMPILED_HEADERS
