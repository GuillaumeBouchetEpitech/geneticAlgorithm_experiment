
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace GlContext {

namespace FrameBuffers {

void genenateMany(uint32_t total, uint32_t* buffers);
void deleteMany(uint32_t total, const uint32_t* buffers);
uint32_t generateOne();
void deleteOne(uint32_t bufferId);
uint32_t getColorAttachment(uint32_t index);
void attachTexture2D(uint32_t index, uint32_t textureId);
void attachDepthTexture2D(uint32_t textureId);
void attachRenderbuffer(uint32_t bufferId);
bool check(bool throwException = true);
void drawMany(uint32_t total, const uint32_t* buffers);
void bind(uint32_t frameBufferId);
void downloadPixels(uint32_t posX, uint32_t posY, uint32_t width,
                    uint32_t height, void* pixels);

} // namespace FrameBuffers

namespace VertexBufferObject {

enum class Primitives {
  lines,
  triangles,
  line_strip,
};

void generateMany(uint32_t total, uint32_t* buffers);
void deleteMany(uint32_t total, const uint32_t* buffers);
void bind(uint32_t vboId);

void enableAttribArray(uint32_t attrId);
void setAttribPointer(uint32_t attrId, uint32_t rowSize, uint32_t stride,
                      uint32_t rowIndex);
void enableAttribDivisor(uint32_t attrId);
void uploadBuffer(const void* data, uint32_t dataSize, bool dynamic);
void drawArrays(Primitives primitive, uint32_t primitiveStart,
                uint32_t primitiveCount);
void drawInstancedArrays(Primitives primitive, uint32_t primitiveStart,
                         uint32_t primitiveCount, uint32_t instanceCount);

} // namespace VertexBufferObject

namespace RenderBuffer {

enum class DepthFormat {
  depth16,
  depth24,
  depth32,
  depth32f,
};

void generateMany(uint32_t total, uint32_t* buffers);
void deleteMany(uint32_t total, const uint32_t* buffers);
void bind(uint32_t renderBufferId);
void setSize(uint32_t width, uint32_t height, DepthFormat depthFormat);

} // namespace RenderBuffer

namespace Shader {

uint32_t loadVertexShader(const std::string& filename,
                          const std::string& source);
uint32_t loadFragmentShader(const std::string& filename,
                            const std::string& source);
uint32_t createProgram();
void deleteProgram(uint32_t programId);
bool linkProgram(uint32_t programId, uint32_t vertexShader,
                 uint32_t fragmentShader);
int getAttribLocation(uint32_t programId, const char* name);
int getUniformLocation(uint32_t programId, const char* name);
void useProgram(uint32_t programId);
void setUniform(int location, int value);
void setUniform(int location, int x, int y);
void setUniform(int location, int x, int y, int z);
void setUniform(int location, int x, int y, int z, int w);
void setUniform(int location, float value);
void setUniform(int location, float x, float y);
void setUniform(int location, float x, float y, float z);
void setUniform(int location, float x, float y, float z, float w);
void setUniform(int location, const glm::mat3& matrix);
void setUniform(int location, const glm::mat4& matrix);

} // namespace Shader

namespace Texture {

enum class DepthFormat {
  depth16,
  depth24,
  depth32,
  depth32f,
};

enum class DepthType {
  unsingedShort,
  unsingedInt,
  float32,
};

void generateMany(uint32_t total, uint32_t* buffers);
void deleteMany(uint32_t total, const uint32_t* buffers);
uint32_t generateOne();
void deleteOne(uint32_t textureId);
void active(uint32_t index);
void bind(uint32_t textureId);
void uploadPixels(uint32_t width, uint32_t height, const void* pixels);
void uploadSingleFloatPixels(uint32_t width, uint32_t height);
void setAsDepthTexture(uint32_t width, uint32_t height, DepthFormat depthFormat,
                       DepthType depthType);
void setTextureAsRepeat(bool repeat);
void setTextureAsPixelated();
void setTextureAsSmoothed(bool generateMipMap = false);

} // namespace Texture

namespace VertexArrayObject {

void generateMany(uint32_t total, uint32_t* buffers);
void deleteMany(uint32_t total, const uint32_t* buffers);
void bind(uint32_t vaoId);
} // namespace VertexArrayObject

std::string getVersion();
void setViewport(int x, int y, uint32_t width, uint32_t height);
void setScissor(int x, int y, uint32_t width, uint32_t height);

enum class DepthFuncs {
  never,
  less,
  equal,
  lessAndEqual,
  greater,
  notEqual,
  greaterAndEqual,
  always,
};
void setDepthFunc(DepthFuncs func);

enum class BlendFuncs {
  srcAlpha,
  oneMinuxSrcAlpha,
};
void setBlendFunc(BlendFuncs sfactor, BlendFuncs dfactor);

enum class States {
  cullFace,
  depthTest,
  blend,
  scissorTest,
};
void enable(States state);
void disable(States state);

enum class BackFaceCullingDirection {
  clockWise,
  counterClockWise,
};
void setBackFaceCullingDirection(BackFaceCullingDirection direction);

enum class Buffers : int {
  color = 1 << 0,
  depth = 1 << 1,
};

void clear(int mask);
void clearColor(float r, float g, float b, float a);
void clearDepth(float value);

// static void polygonModeAsLine();
// static void polygonModeAsFill();

int getMaxTextureSize();
}; // namespace GlContext
