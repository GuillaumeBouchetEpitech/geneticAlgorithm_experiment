
#include "framework/system/compression/AdaptiveHuffmanAlgorithm.hpp"

#include "framework/system/TraceLogger.hpp"

#include <memory>

void test_compression() {
  D_MYLOG("test_compression()");

  {
    constexpr int k_buffSize = 256;
    constexpr int k_offset = 0;

    auto smartBufferPrimary = std::make_unique<char[]>(k_buffSize);
    char* rawBufferPrimary = smartBufferPrimary.get();

    for (int ii = 0; ii < k_buffSize; ++ii) {
      rawBufferPrimary[ii] = char(ii % 10);
    }

    auto smartBufferSecondary = std::make_unique<char[]>(k_buffSize);
    char* rawBufferSecondary = smartBufferSecondary.get();
    std::memcpy(rawBufferSecondary, rawBufferPrimary, k_buffSize);

    int tmpBuffSize = k_buffSize;

    // D_MYLOG("[raw] tmpBuffSize " << tmpBuffSize);

    const bool success = AdaptiveHuffman::compress(rawBufferSecondary, tmpBuffSize, k_offset);

    assert(success == true);
    assert(tmpBuffSize < k_buffSize);

    // D_MYLOG("[compressed] success " << success);
    // D_MYLOG("[compressed] tmpBuffSize " << tmpBuffSize);

    AdaptiveHuffman::decompress(rawBufferSecondary, tmpBuffSize, k_buffSize, k_offset);

    assert(tmpBuffSize == k_buffSize);

    // D_MYLOG("[decompressed] tmpBuffSize " << tmpBuffSize);

    for (int ii = 0; ii < tmpBuffSize; ++ii) {
      assert(rawBufferSecondary[ii] == rawBufferPrimary[ii]);
    }
  }

  {
    constexpr int k_buffSize = 256;
    constexpr int k_offset = 0;

    auto smartBufferPrimary = std::make_unique<char[]>(k_buffSize);
    char* rawBufferPrimary = smartBufferPrimary.get();

    for (int ii = 0; ii < k_buffSize; ++ii) {
      rawBufferPrimary[ii] = char(ii % 50);
    }

    auto smartBufferSecondary = std::make_unique<char[]>(k_buffSize);
    char* rawBufferSecondary = smartBufferSecondary.get();
    std::memcpy(rawBufferSecondary, rawBufferPrimary, k_buffSize);

    int tmpBuffSize = k_buffSize;

    // D_MYLOG("[raw] tmpBuffSize " << tmpBuffSize);

    const bool success = AdaptiveHuffman::compress(rawBufferSecondary, tmpBuffSize, k_offset);

    assert(success == true);
    assert(tmpBuffSize < k_buffSize);

    // D_MYLOG("[compressed] success " << success);
    // D_MYLOG("[compressed] tmpBuffSize " << tmpBuffSize);

    AdaptiveHuffman::decompress(rawBufferSecondary, tmpBuffSize, k_buffSize, k_offset);

    assert(tmpBuffSize == k_buffSize);

    // D_MYLOG("[decompressed] tmpBuffSize " << tmpBuffSize);

    for (int ii = 0; ii < tmpBuffSize; ++ii) {
      assert(rawBufferSecondary[ii] == rawBufferPrimary[ii]);
    }
  }

  {
    constexpr int k_buffSize = 256;
    constexpr int k_offset = 0;

    auto smartBufferPrimary = std::make_unique<char[]>(k_buffSize);
    char* rawBufferPrimary = smartBufferPrimary.get();

    for (int ii = 0; ii < k_buffSize; ++ii) {
      rawBufferPrimary[ii] = char(ii % 100);
    }

    auto smartBufferSecondary = std::make_unique<char[]>(k_buffSize);
    char* rawBufferSecondary = smartBufferSecondary.get();
    std::memcpy(rawBufferSecondary, rawBufferPrimary, k_buffSize);

    int tmpBuffSize = k_buffSize;

    // D_MYLOG("[raw] tmpBuffSize " << tmpBuffSize);

    const bool success = AdaptiveHuffman::compress(rawBufferSecondary, tmpBuffSize, k_offset);

    assert(success == false);
    assert(tmpBuffSize == k_buffSize);

    // D_MYLOG("[compressed] success " << success);
    // D_MYLOG("[compressed] tmpBuffSize " << tmpBuffSize);

    for (int ii = 0; ii < tmpBuffSize; ++ii) {
      assert(rawBufferSecondary[ii] == rawBufferPrimary[ii]);
    }
  }

  D_MYLOG(" => DONE");
}
