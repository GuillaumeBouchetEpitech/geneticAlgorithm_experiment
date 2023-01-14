
#include "headers.hpp"

TEST(compression,
     easy__can_compress_to_smaller_size_and_decompress_to_original) {
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

  const bool success =
    AdaptiveHuffman::compress(rawBufferSecondary, tmpBuffSize, k_offset);

  EXPECT_EQ(success, true);
  EXPECT_LT(tmpBuffSize, k_buffSize);

  AdaptiveHuffman::decompress(rawBufferSecondary, tmpBuffSize, k_buffSize,
                              k_offset);

  EXPECT_EQ(tmpBuffSize, k_buffSize);

  for (int ii = 0; ii < tmpBuffSize; ++ii) {
    EXPECT_EQ(rawBufferSecondary[ii], rawBufferPrimary[ii]);
  }
}

TEST(compression,
     difficult__can_compress_to_smaller_size_and_decompress_to_original) {
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

  const bool success =
    AdaptiveHuffman::compress(rawBufferSecondary, tmpBuffSize, k_offset);

  EXPECT_EQ(success, true);
  EXPECT_LT(tmpBuffSize, k_buffSize);

  AdaptiveHuffman::decompress(rawBufferSecondary, tmpBuffSize, k_buffSize,
                              k_offset);

  EXPECT_EQ(tmpBuffSize, k_buffSize);

  for (int ii = 0; ii < tmpBuffSize; ++ii) {
    EXPECT_EQ(rawBufferSecondary[ii], rawBufferPrimary[ii]);
  }
}

TEST(compression, can_fail_to_compress_and_just_return_false) {
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

  const bool success =
    AdaptiveHuffman::compress(rawBufferSecondary, tmpBuffSize, k_offset);

  EXPECT_EQ(success, false);
  EXPECT_EQ(tmpBuffSize, k_buffSize);

  for (int ii = 0; ii < tmpBuffSize; ++ii) {
    EXPECT_EQ(rawBufferSecondary[ii], rawBufferPrimary[ii]);
  }
}
