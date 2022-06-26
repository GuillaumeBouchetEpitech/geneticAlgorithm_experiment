
#include "AbstractMessage.hpp"

AbstractMessage::AbstractMessage(const char* dataPointer, unsigned int dataSize)
    : _dataPointer(const_cast<char*>(dataPointer))
    , _dataSize(dataSize)
{}

void AbstractMessage::clear()
{
    _dataPointer = nullptr;
    _dataSize = 0;
    _readIndex = 0;
}

const char* AbstractMessage::getData() const
{
    return _dataPointer;
}

unsigned int AbstractMessage::getSize() const
{
    return _dataSize;
}
