
#include "ogg.hpp"

#include "framework/ErrorHandler.hpp"
#include "framework/TraceLogger.hpp"

#include "framework/helpers/internals/STBvorbis.hpp"

namespace decoders {

namespace ogg {

char* decode(const std::string& content, std::int32_t& outChannels,
             std::int32_t& outSampleRate, std::int32_t& outBitsPerSample,
             uint32_t& outSize) {

  // https://cpp.hotexamples.com/examples/-/-/stb_vorbis_decode_memory/cpp-stb_vorbis_decode_memory-function-examples.html

  std::int32_t channels, sampleRate;
  short* soundSamples;

  const std::int32_t numSamples = stb_vorbis_decode_memory(
    reinterpret_cast<const unsigned char*>(content.data()), int(content.size()),
    &channels, &sampleRate, &soundSamples);

  if (numSamples < 0) {
    D_THROW(std::runtime_error, "ERROR: Could not decode ogg file");
  }

  outChannels = channels;
  outSampleRate = sampleRate;
  outBitsPerSample = 16;
  outSize =
    uint32_t(numSamples * channels * std::int32_t(sizeof(std::int16_t)));

  // D_MYLOG("numSamples " << numSamples);
  // D_MYLOG("outChannels " << outChannels);
  // D_MYLOG("outSampleRate " << outSampleRate);
  // D_MYLOG("outBitsPerSample " << outBitsPerSample);
  // D_MYLOG("outSize " << outSize);

  return reinterpret_cast<char*>(soundSamples);
}

void deallocate(char** samples) {
  free(*samples);
  *samples = nullptr;
}

} // namespace ogg

} // namespace decoders
