
#pragma once

#include "thirdparty/GLMath.hpp"

#include <string>

class AbstractMessage
{
protected:
    char* _dataPointer = nullptr;
    unsigned int _dataSize = 0;
    unsigned int _readIndex = 0;

public:
    AbstractMessage() = default;
    AbstractMessage(const char* dataPointer, unsigned int dataSize);
    virtual ~AbstractMessage() = default;

public:
    virtual void clear();

public:
    const char* getData() const;
    unsigned int getSize() const;

};
