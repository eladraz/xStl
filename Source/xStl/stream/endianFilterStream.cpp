#include "xStl/xStlPrecompiled.h"
/*
 * endianFilterStream.cpp
 *
 * Implementation file.
 *
 *  Elad Raz
 */
#include "xStl/types.h"
#include "xStl/data/array.h"
#include "xStl/except/exception.h"
#include "xStl/except/assert.h"
#include "xStl/stream/basicIO.h"
#include "xStl/stream/filterStream.h"
#include "xStl/stream/endianFilterStream.h"


cEndianFilterStream::cEndianFilterStream(const cSmartPtr<basicIO>& stream,
                                         bool isLittleEndian) :
    filterStream(stream),
    m_isLittleEndian(isLittleEndian)
{
    initSeek();
}

void cEndianFilterStream::initSeek()
{
    this->canLength = m_stream->canLength;
    this->canSeekFromBegin = m_stream->canSeekFromBegin;
    this->canSeekForward = m_stream->canSeekForward;
    this->canSeekBackward = m_stream->canSeekBackward;
    this->canSeekFromEnd = m_stream->canSeekFromEnd;
    this->canGetPointer = m_stream->canGetPointer;
}

cEndianFilterStream::~cEndianFilterStream()
{
    flush();
}

uint cEndianFilterStream::length() const 
{
    return m_stream->length();
}

uint cEndianFilterStream::getPointer() const
{
    return m_stream->getPointer();
}

void cEndianFilterStream::flush()
{
    m_stream->flush();
}

uint cEndianFilterStream::write(const void *buffer, const uint length)
{
    return m_stream->write(buffer, length);
}

uint cEndianFilterStream::read(void *buffer, const uint length)
{
    return m_stream->read(buffer, length);;
}

void cEndianFilterStream::seek(const int distance, const basicInput::seekMethod method)
{
    return m_stream->seek(distance, method);;
}

bool cEndianFilterStream::isEOS()
{
    return m_stream->isEOS();
}

uint cEndianFilterStream::getPipeReadBestRequest() const
{
    return m_stream->getPipeReadBestRequest();
}

//////////////////////////////////////////////////////////

uint64 cEndianFilterStream::readUint64(const uint8* buffer) const 
{ 
    return cEndian::readUint64(buffer, m_isLittleEndian); 
}

uint32 cEndianFilterStream::readUint32(const uint8* buffer) const 
{ 
    return cEndian::readUint32(buffer, m_isLittleEndian); 
};

uint16 cEndianFilterStream::readUint16(const uint8* buffer) const 
{ 
    return cEndian::readUint16(buffer, m_isLittleEndian); 
};

void cEndianFilterStream::writeUint64(uint8* buffer, uint64 number) const 
{ 
    cEndian::writeUint64(buffer,number, m_isLittleEndian); 
}; 

void cEndianFilterStream::writeUint32(uint8* buffer, uint32 number) const 
{ 
    cEndian::writeUint32(buffer,number, m_isLittleEndian); 
}; 

void cEndianFilterStream::writeUint16(uint8* buffer, uint16 number) const 
{ 
    cEndian::writeUint16(buffer,number, m_isLittleEndian); 
};

bool cEndianFilterStream::isLittleEndian() const 
{ 
    return m_isLittleEndian;
}
