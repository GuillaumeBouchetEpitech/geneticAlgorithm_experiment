
#pragma once

#include "AbstractMessage.hpp"

#include "framework/helpers/GLMath.hpp"

#include <string>

class MessageView
  : public AbstractMessage
{
private:
  unsigned int _readIndex = 0;

public:
  MessageView() = default;
  MessageView(const char* dataPointer, unsigned int size);

public:
  virtual void clear() override;

public:
  MessageView& read(void* dataPointer, unsigned int size);

public:
  MessageView& operator >>(bool& data);
  MessageView& operator >>(char& data);
  MessageView& operator >>(unsigned char& data);
  MessageView& operator >>(short& data);
  MessageView& operator >>(unsigned short& data);
  MessageView& operator >>(int& data);
  MessageView& operator >>(unsigned int& data);
  MessageView& operator >>(long& data);
  MessageView& operator >>(unsigned long& data);
  MessageView& operator >>(float& data);
  MessageView& operator >>(double& data);
  MessageView& operator >>(std::string& data);
  MessageView& operator >>(glm::vec3& data);
  MessageView& operator >>(glm::vec4& data);
  MessageView& operator >>(glm::mat4& data);

};
