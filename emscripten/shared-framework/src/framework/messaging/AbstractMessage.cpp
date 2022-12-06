
#include "AbstractMessage.hpp"

AbstractMessage::AbstractMessage(const char* dataPointer, uint32_t dataSize)
  : _dataPointer(const_cast<char*>(dataPointer)), _dataSize(dataSize) {}

void AbstractMessage::clear() {
  _dataPointer = nullptr;
  _dataSize = 0;
}

const char* AbstractMessage::getData() const { return _dataPointer; }

uint32_t AbstractMessage::getSize() const { return _dataSize; }
