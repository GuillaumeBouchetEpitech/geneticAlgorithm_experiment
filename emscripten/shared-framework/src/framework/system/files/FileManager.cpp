
#include "FileManager.hpp"

#include "getFileContent.hpp"

#include "framework/system/ErrorHandler.hpp"

#include <cstring>

DataBuffer::DataBuffer(const std::uint8_t* inData, std::size_t inSize) {
  _size = inSize + 1;
  _data = std::make_unique<std::uint8_t[]>(_size);
  std::memset(_data.get(), 0, _size);
  std::memcpy(_data.get(), inData, inSize);
}

DataBuffer::DataBuffer(const std::int8_t* inData, std::size_t inSize)
  : DataBuffer(reinterpret_cast<const std::uint8_t*>(inData), inSize) {}

DataBuffer::DataBuffer(const std::uint8_t* str)
  : DataBuffer(str, strlen(reinterpret_cast<const char*>(str))) {}

DataBuffer::DataBuffer(const std::int8_t* str)
  : DataBuffer(str, strlen(reinterpret_cast<const char*>(str))) {}

DataBuffer::DataBuffer(const std::string& str)
  : DataBuffer(reinterpret_cast<const std::uint8_t*>(str.c_str()), str.size()) {
}

DataBuffer::DataBuffer(DataBuffer&& other) // allow move
{
  std::swap(_size, other._size);
  std::swap(_data, other._data);
}

DataBuffer& DataBuffer::operator=(DataBuffer&& other) // allow move
{
  if (&other == this)
    return *this;
  std::swap(_size, other._size);
  std::swap(_data, other._data);
  return *this;
}

const unsigned char* DataBuffer::getUCharData() const { return _data.get(); }
const char* DataBuffer::getCharData() const {
  return reinterpret_cast<char*>(_data.get());
}
std::size_t DataBuffer::getSize() const { return _size; }
const std::string_view DataBuffer::asStringView() const {
  return std::string_view(getCharData(), _size - 1);
}
void DataBuffer::fillString(std::string& outFileContent) const {
  outFileContent.assign(getCharData(), getSize() - 1);
}

void FileManager::load(const std::string& filename) {
  if (_buffersMap.count(filename) > 0)
    return;

  std::string fileContent;
  if (!FileUtils::getFileContent(filename, fileContent)) {
    D_THROW(std::runtime_error,
            "ERROR: Could not load file: \"" << filename << "\"");
  }

  auto ref = _buffersPool.acquire(fileContent);

  if (!ref) {
    D_THROW(std::runtime_error, "ERROR: pool is full");
  }

  _buffersMap[filename] = ref;
}

FileManager::BufferWeakRef
FileManager::getDataBuffer(const std::string& filename) {
  auto it = _buffersMap.find(filename);
  if (it == _buffersMap.end())
    return FileManager::BufferWeakRef::make_invalid();
  return it->second;
}

namespace FileUtils {

LoadCallback getFileManagerCallback(FileManager& fileManager) {
  return
    [&fileManager](const std::string& filename, std::string& outFileContent) {
      outFileContent.clear();
      fileManager.load(filename);
      auto ref = fileManager.getDataBuffer(filename);
      ref->fillString(outFileContent);
    };
}

} // namespace FileUtils
