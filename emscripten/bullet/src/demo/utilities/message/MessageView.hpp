
#pragma once

#include "thirdparty/GLMath.hpp"

#include <string>

class MessageView
{
protected:
    char* _dataPointer = nullptr;
    unsigned int _dataSize = 0;
    unsigned int _readIndex = 0;

public:
    MessageView() = default;
    MessageView(const char* pData, unsigned int size);
    virtual ~MessageView();

public:
    MessageView& read(void* pData, unsigned int size);

    virtual void clear();

public:
    void resetReadIndex();
    // bool verifySizeLeft(unsigned int size) const;
    const char* getData() const;
    unsigned int getSize() const;

public:
    MessageView& operator >>(bool& data);
    MessageView& operator >>(char& data);
    MessageView& operator >>(unsigned char& data);
    MessageView& operator >>(short& data);
    MessageView& operator >>(unsigned short& data);
    MessageView& operator >>(int& data);
    MessageView& operator >>(unsigned int& data);
    MessageView& operator >>(float& data);
    MessageView& operator >>(double& data);
    MessageView& operator >>(std::string& data);
    MessageView& operator >>(glm::vec3& data);
    MessageView& operator >>(glm::mat4& data);

};
