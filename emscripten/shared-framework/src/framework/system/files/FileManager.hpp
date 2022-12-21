
#pragma once

#include "FileLoadFileCallback.hpp"

#include "framework/system/containers/weak_ref_data_pool.hpp"

#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>

class DataBuffer {
private:
  std::size_t _size;
  std::unique_ptr<std::uint8_t[]> _data;

public:
  DataBuffer(const std::uint8_t* inData, std::size_t inSize);
  DataBuffer(const std::int8_t* inData, std::size_t inSize);
  DataBuffer(const std::uint8_t* str);
  DataBuffer(const std::int8_t* str);
  DataBuffer(const std::string& str);

  virtual ~DataBuffer() = default;

public:
  DataBuffer(const DataBuffer& other) = delete;            // no copy
  DataBuffer& operator=(const DataBuffer& other) = delete; // no copy

public:
  DataBuffer(DataBuffer&& other);            // allow move
  DataBuffer& operator=(DataBuffer&& other); // allow move

public:
  const unsigned char* getUCharData() const;
  const char* getCharData() const;
  std::size_t getSize() const;
  const std::string_view asStringView() const;
  void fillString(std::string& outFileContent) const;
};

class FileManager {
public:
  using BuffersPool = weak_ref_data_pool<DataBuffer, DataBuffer, 256, false>;
  using BufferWeakRef = BuffersPool::weak_ref;

private:
  BuffersPool _buffersPool;
  std::unordered_map<std::string, BufferWeakRef> _buffersMap;

public:
  FileManager() = default;
  ~FileManager() = default;

public:
  void load(const std::string& filename);

  BufferWeakRef getDataBuffer(const std::string& filename);
};

namespace FileUtils {

LoadCallback getFileManagerCallback(FileManager& fileManager);

}
