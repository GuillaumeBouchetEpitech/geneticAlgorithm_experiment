
#pragma once

#include <cstdint>

enum class ShaderIds : uint32_t {
  stackRendererHud = 0,
  stackRendererScene,
  particles,
  simpleTexture,
  simpleTextureLit,
  hudText,
  wireframes,
  animatedCircuit,
  animatedCircuitLit,
  models,
  modelsLit,
  postProcess,
  backGroundTorus,
};

enum class GeometryIds : uint32_t {
  stackRendererWireframesHud = 0,
  stackRendererTrianglesHud,
  stackRendererWireframesScene,
  stackRendererTrianglesScene,
  particles,
  simpleTexture,
  simpleTextureLit,
  hudText,
  wireframes,
  wireframesLineStrip,
  animatedCircuit,
  animatedCircuitLit,
  models,
  modelsLit,
  // postProcess,
  backGroundTorus,
};

// enum class Textures : uint32_t
// {
//   asciiFont = 0,
// };
