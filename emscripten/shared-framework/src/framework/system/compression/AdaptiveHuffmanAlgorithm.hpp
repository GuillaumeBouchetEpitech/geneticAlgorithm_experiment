
#pragma once

namespace AdaptiveHuffman {

void decompress(char* inData, int& inOutSize, int inMaxSize, int inOffset);
bool compress(char* inData, int& inOutSize, int inOffset);

} // namespace AdaptiveHuffman
