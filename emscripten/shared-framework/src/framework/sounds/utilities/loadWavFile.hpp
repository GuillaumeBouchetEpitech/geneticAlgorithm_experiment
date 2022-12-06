
#pragma once

#include <AL/al.h>
#include <AL/alc.h>

#include <string>

char* load_wav(const std::string& filename, std::uint8_t& channels,
               std::int32_t& sampleRate, std::uint8_t& bitsPerSample,
               ALsizei& size);
