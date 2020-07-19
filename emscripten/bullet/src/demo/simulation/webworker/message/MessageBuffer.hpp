
#pragma once

#include "MessageView.hpp"

#include <vector>

class MessageBuffer
    : public MessageView
{
protected:
    std::vector<char> _dataContainer;

public:
    MessageBuffer();

public:
    MessageBuffer& append(const void* pData, unsigned int size);

public:
    virtual void clear() override;

public:
    MessageBuffer& operator <<(bool data);
    MessageBuffer& operator <<(char data);
    MessageBuffer& operator <<(unsigned char data);
    MessageBuffer& operator <<(short data);
    MessageBuffer& operator <<(unsigned short data);
    MessageBuffer& operator <<(int data);
    MessageBuffer& operator <<(unsigned int data);
    MessageBuffer& operator <<(long data);
    MessageBuffer& operator <<(unsigned long data);
    MessageBuffer& operator <<(float data);
    MessageBuffer& operator <<(double data);
    MessageBuffer& operator <<(const std::string& data);
    MessageBuffer& operator <<(const glm::vec3& data);
    MessageBuffer& operator <<(const glm::vec4& data);
    MessageBuffer& operator <<(const glm::mat4& data);

};
