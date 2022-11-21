
#pragma once

#include <vector>

class VertexBufferObject
{
private:
  std::vector<unsigned int> _ids;

public:
  ~VertexBufferObject();

public:
  void allocate(std::size_t size = 1);
  void deallocate();

public:
  bool isAllocated() const;

public:
  void bind(unsigned int index = 0) const;

public:
  static void unbind();
};
