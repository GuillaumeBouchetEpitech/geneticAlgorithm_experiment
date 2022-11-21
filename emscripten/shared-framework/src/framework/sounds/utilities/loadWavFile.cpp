
#include "loadWavFile.hpp"

#include "framework/ErrorHandler.hpp"

#include <cstring>
#include <fstream>

namespace /*anonymous*/
{
  bool isLittleEndian()
  {
    short value = 1;
    return reinterpret_cast<char*>(&value)[0] == 1;
  }

  std::int32_t convert_to_int(char* buffer, std::size_t len)
  {
    std::int32_t a = 0;
    if (isLittleEndian())
    {
      std::memcpy(&a, buffer, len);
    }
    else
    {
      for (std::size_t i = 0; i < len; ++i)
        reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
    }
    return a;
  }

    bool load_wav_file_header(std::ifstream& fileStream,
                              std::uint8_t& channels,
                              std::int32_t& sampleRate,
                              std::uint8_t& bitsPerSample,
                              ALsizei& size)
    {
      char buffer[4];

      if (!fileStream.is_open())
        return false;

      // the RIFF
      if (!fileStream.read(buffer, 4))
        D_THROW(std::runtime_error, "ERROR: could not read RIFF");
      if (std::strncmp(buffer, "RIFF", 4) != 0)
        D_THROW(std::runtime_error, "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)");

      // the size of the file
      if (!fileStream.read(buffer, 4))
        D_THROW(std::runtime_error, "ERROR: could not read size of file");

      // the WAVE
      if (!fileStream.read(buffer, 4))
        D_THROW(std::runtime_error, "ERROR: could not read WAVE");
      if (std::strncmp(buffer, "WAVE", 4) != 0)
        D_THROW(std::runtime_error, "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)");

      // "fmt/0"
      if (!fileStream.read(buffer, 4))
        D_THROW(std::runtime_error, "ERROR: could not read fmt/0");

      // this is always 16, the size of the fmt data chunk
      if (!fileStream.read(buffer, 4))
        D_THROW(std::runtime_error, "ERROR: could not read the 16");

      // PCM should be 1?
      if (!fileStream.read(buffer, 2))
        D_THROW(std::runtime_error, "ERROR: could not read PCM");

      // the number of channels
      if (!fileStream.read(buffer, 2))
        D_THROW(std::runtime_error, "ERROR: could not read number of channels");
      channels = convert_to_int(buffer, 2);

      // sample rate
      if (!fileStream.read(buffer, 4))
        D_THROW(std::runtime_error, "ERROR: could not read sample rate");
      sampleRate = convert_to_int(buffer, 4);

      // (sampleRate * bitsPerSample * channels) / 8
      if (!fileStream.read(buffer, 4))
        D_THROW(std::runtime_error, "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8");

      // ?? dafaq
      if (!fileStream.read(buffer, 2))
        D_THROW(std::runtime_error, "ERROR: could not read dafaq");

      // bitsPerSample
      if (!fileStream.read(buffer, 2))
        D_THROW(std::runtime_error, "ERROR: could not read bits per sample");
      bitsPerSample = convert_to_int(buffer, 2);

      // data chunk header "data"
      if (!fileStream.read(buffer, 4))
        D_THROW(std::runtime_error, "ERROR: could not read data chunk header");
      if (std::strncmp(buffer, "data", 4) != 0)
        D_THROW(std::runtime_error, "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)");

      // size of data
      if (!fileStream.read(buffer, 4))
        D_THROW(std::runtime_error, "ERROR: could not read data size");
      size = convert_to_int(buffer, 4);

      /* cannot be at the end of file */
      if (fileStream.eof())
        D_THROW(std::runtime_error, "ERROR: reached EOF on the file");
      if (fileStream.fail())
        D_THROW(std::runtime_error, "ERROR: fail state set on the file");

      return true;
    }

};

char* load_wav(const std::string& filename,
               std::uint8_t& channels,
               std::int32_t& sampleRate,
               std::uint8_t& bitsPerSample,
               ALsizei& size)
{
  std::ifstream fileStream(filename, std::ios::binary);

  if (!fileStream.is_open())
    D_THROW(std::runtime_error, "ERROR: Could not open \"" << filename << "\"");

  if (!load_wav_file_header(fileStream, channels, sampleRate, bitsPerSample, size))
    D_THROW(std::runtime_error, "ERROR: Could not load wav header of \"" << filename << "\"");

  char* data = new char[size];

  fileStream.read(data, size);

  return data;
}
