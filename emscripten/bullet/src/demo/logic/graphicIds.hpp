
#pragma once

#include <cstdint>

enum class Shaders : uint32_t
{
  stackRenderer = 0,
  particles = 1,
  simpleTexture = 2,
  litTexture = 3,
  hudText = 4,
  wireframes = 5,
  animatedCircuit = 6,
  animatedCircuitLit = 7,
  models = 8,
};

enum class Geometries : uint32_t
{
  stackRenderer = 0,
  particles = 1,
  simpleTexture = 2,
  hudText = 3,
  wireframes = 4,
  animatedCircuit = 5,
  models = 6,
};

// enum class Textures : uint32_t
// {
//   asciiFont = 0,
// };
