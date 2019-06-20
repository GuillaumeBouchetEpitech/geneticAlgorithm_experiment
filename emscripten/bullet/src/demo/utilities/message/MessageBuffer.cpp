
#include "MessageBuffer.hpp"

#include "demo/utilities/ErrorHandler.hpp"
// #include "demo/utilities/TraceLogger.hpp"

#include <cstring> // <= std::memcpy

MessageBuffer::MessageBuffer()
    : MessageView()
{
    /**
     * grow the cached memory block
     * => it reallocate when needed, will only grow () (not on it's own anyway)
     * => it do so with a power of 2 value (1, 2, 4, 8, 16, etc.)
     * => starting it at a "high" value (512) make that part faster
     */
    _dataContainer.reserve(512);
}

MessageBuffer&    MessageBuffer::append(const void* dataPointer, unsigned int dataSize)
{
    if (dataPointer && dataSize > 0)
    {
        // this will cause a reallocation if not large enough
        _dataContainer.resize(_dataSize + dataSize);

        /**
         * this is done in case _dataContainer needed to reallocate
         * => the pointer value would differ
         */
        _dataPointer = _dataContainer.data();

        // _dataSize is holding it's pre-resize value
        std::memcpy(_dataPointer + _dataSize, dataPointer, dataSize);

        // update the _dataSize value
        _dataSize = _dataContainer.size();
    }

    return *this;
}

void    MessageBuffer::clear()
{
    MessageView::clear();
    _dataContainer.clear(); // <= clear the used size but keep it's (cached) capacity
}

MessageBuffer&  MessageBuffer::operator <<(bool data)
{
    *this << char(data); // <= write like a char
    return *this;
}

MessageBuffer&  MessageBuffer::operator <<(char data)
{
    return append(&data, sizeof(data));
}

MessageBuffer&  MessageBuffer::operator <<(unsigned char data)
{
    return append(&data, sizeof(data));
}

MessageBuffer&  MessageBuffer::operator <<(short data)
{
    return append(&data, sizeof(data));
}

MessageBuffer&  MessageBuffer::operator <<(unsigned short data)
{
    return append(&data, sizeof(data));
}

MessageBuffer&  MessageBuffer::operator <<(int data)
{
    return append(&data, sizeof(data));
}

MessageBuffer&  MessageBuffer::operator <<(unsigned int data)
{
    return append(&data, sizeof(data));
}

MessageBuffer&  MessageBuffer::operator <<(float data)
{
    return append(&data, sizeof(data));
}

MessageBuffer&  MessageBuffer::operator <<(double data)
{
    return append(&data, sizeof(data));
}

MessageBuffer&	MessageBuffer::operator <<(const std::string& data)
{
    if (data.empty())
        D_THROW(std::runtime_error, "can't append an empty string");

    unsigned int length = data.size();
    const char* rawString = data.c_str();
    append(&length, sizeof(length));

    append(rawString, length * sizeof(*rawString));

    return *this;
}

MessageBuffer&	MessageBuffer::operator <<(const glm::vec3& data)
{
	return append(&data.x, 3 * sizeof(float));
}

MessageBuffer&	MessageBuffer::operator <<(const glm::mat4& data)
{
	return append(glm::value_ptr(data), 16 * sizeof(float));
}
