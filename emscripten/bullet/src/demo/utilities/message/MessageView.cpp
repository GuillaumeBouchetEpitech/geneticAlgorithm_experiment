
#include "MessageView.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include <memory> // <= std::make_unique
#include <cstring> // <= std::memcpy

MessageView::MessageView(const char* dataPointer, unsigned int dataSize)
    : _dataPointer(const_cast<char*>(dataPointer))
    , _dataSize(dataSize)
{}

MessageView::~MessageView()
{}

//

MessageView&    MessageView::read(void* dataPointer, unsigned int dataSize)
{
    // verify size left
    if (_readIndex + dataSize > _dataSize)
        D_THROW(std::runtime_error, "can't read, not enough size left" 
									<< ", current index=" << _readIndex
									<< ", next index=" << (_readIndex + dataSize)
									<< ", data size=" << _dataSize);

    std::memcpy(dataPointer, _dataPointer + _readIndex, dataSize);
    _readIndex += dataSize;
    return *this;
}

void    MessageView::clear()
{
    _dataPointer = nullptr;
    _dataSize = 0;
    _readIndex = 0;
}

void	MessageView::resetReadIndex()
{
	_readIndex = 0;
}

const char*  MessageView::getData() const
{
	return _dataPointer;
}

unsigned int MessageView::getSize() const
{
	return _dataSize;
}

//

MessageView&  MessageView::operator >>(bool& data)
{
    char value = 0;
    read(&value, sizeof(char)); // <= read like a char
    data = (value != 0);
    return *this;
}

MessageView&	MessageView::operator >>(char& data)
{
	return read(&data, sizeof(data));
}

MessageView&	MessageView::operator >>(unsigned char& data)
{
	return read(&data, sizeof(data));
}

MessageView&	MessageView::operator >>(short& data)
{
	return read(&data, sizeof(data));
}

MessageView&	MessageView::operator >>(unsigned short& data)
{
	return read(&data, sizeof(data));
}

MessageView&	MessageView::operator >>(int& data)
{
	return read(&data, sizeof(data));
}

MessageView&	MessageView::operator >>(unsigned int& data)
{
	return read(&data, sizeof(data));
}

MessageView&	MessageView::operator >>(float& data)
{
	return read(&data, sizeof(data));
}

MessageView&	MessageView::operator >>(double& data)
{
	return read(&data, sizeof(data));
}

MessageView&	MessageView::operator >>(std::string& data)
{
	unsigned int length = 0;
	read(&length, sizeof(length));

	auto buffer = std::make_unique<char[]>(length + 1);
    char* rawBuffer = buffer.get();
	std::memset(rawBuffer, 0, length + 1);
	read(rawBuffer, length * sizeof(*rawBuffer));

	data = rawBuffer;

	return *this;
}

MessageView&	MessageView::operator >>(glm::vec3& data)
{
	return read(&data.x, 3 * sizeof(float));
}

MessageView&	MessageView::operator >>(glm::mat4& data)
{
	return read(glm::value_ptr(data), 16 * sizeof(float));
}
