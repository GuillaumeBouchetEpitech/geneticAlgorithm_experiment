
#pragma once

#include <string>

namespace decoders {

namespace ogg {

char* decode(const std::string& filename, std::int32_t& channels,
             std::int32_t& sampleRate, std::int32_t& bitsPerSample,
             uint32_t& size);

void deallocate(char** samples);

} // namespace ogg

} // namespace decoders
