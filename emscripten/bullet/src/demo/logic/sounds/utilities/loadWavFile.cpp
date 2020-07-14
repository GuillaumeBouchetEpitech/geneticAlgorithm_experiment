
#include "loadWavFile.hpp"

#include "demo/utilities/ErrorHandler.hpp"

#include <cstring>
#include <fstream>

namespace /*anonymous*/
{

    std::int32_t convert_to_int(char* buffer, std::size_t len)
    {
        std::int32_t a = 0;
        // if (std::endian::native == std::endian::little)
            std::memcpy(&a, buffer, len);
        // else
        //     for (std::size_t i = 0; i < len; ++i)
        //         reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
        return a;
    }

    bool load_wav_file_header(std::ifstream& file,
                              std::uint8_t& channels,
                              std::int32_t& sampleRate,
                              std::uint8_t& bitsPerSample,
                              ALsizei& size)
    {
        char buffer[4];
        if (!file.is_open())
            return false;

        // the RIFF
        if (!file.read(buffer, 4))
            D_THROW(std::runtime_error, "ERROR: could not read RIFF");
        if (std::strncmp(buffer, "RIFF", 4) != 0)
            D_THROW(std::runtime_error, "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)");

        // the size of the file
        if (!file.read(buffer, 4))
            D_THROW(std::runtime_error, "ERROR: could not read size of file");

        // the WAVE
        if (!file.read(buffer, 4))
            D_THROW(std::runtime_error, "ERROR: could not read WAVE");
        if (std::strncmp(buffer, "WAVE", 4) != 0)
            D_THROW(std::runtime_error, "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)");

        // "fmt/0"
        if (!file.read(buffer, 4))
            D_THROW(std::runtime_error, "ERROR: could not read fmt/0");

        // this is always 16, the size of the fmt data chunk
        if (!file.read(buffer, 4))
            D_THROW(std::runtime_error, "ERROR: could not read the 16");

        // PCM should be 1?
        if (!file.read(buffer, 2))
            D_THROW(std::runtime_error, "ERROR: could not read PCM");

        // the number of channels
        if (!file.read(buffer, 2))
            D_THROW(std::runtime_error, "ERROR: could not read number of channels");
        channels = convert_to_int(buffer, 2);

        // sample rate
        if (!file.read(buffer, 4))
            D_THROW(std::runtime_error, "ERROR: could not read sample rate");
        sampleRate = convert_to_int(buffer, 4);

        // (sampleRate * bitsPerSample * channels) / 8
        if (!file.read(buffer, 4))
            D_THROW(std::runtime_error, "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8");

        // ?? dafaq
        if (!file.read(buffer, 2))
            D_THROW(std::runtime_error, "ERROR: could not read dafaq");

        // bitsPerSample
        if (!file.read(buffer, 2))
            D_THROW(std::runtime_error, "ERROR: could not read bits per sample");
        bitsPerSample = convert_to_int(buffer, 2);

        // data chunk header "data"
        if (!file.read(buffer, 4))
            D_THROW(std::runtime_error, "ERROR: could not read data chunk header");
        if (std::strncmp(buffer, "data", 4) != 0)
            D_THROW(std::runtime_error, "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)");

        // size of data
        if (!file.read(buffer, 4))
            D_THROW(std::runtime_error, "ERROR: could not read data size");
        size = convert_to_int(buffer, 4);

        /* cannot be at the end of file */
        if (file.eof())
            D_THROW(std::runtime_error, "ERROR: reached EOF on the file");
        if (file.fail())
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
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open())
        D_THROW(std::runtime_error, "ERROR: Could not open \"" << filename << "\"");
    if (!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
        D_THROW(std::runtime_error, "ERROR: Could not load wav header of \"" << filename << "\"");

    char* data = new char[size];

    in.read(data, size);

    return data;
}
