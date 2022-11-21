
#pragma once

#include "framework/helpers/GLMath.hpp"

#include <string>
#include <cstdint>

class GlContext
{
public:
  enum class Primitives
  {
    lines,
    triangles,
    line_strip,
  };

public:
  static void genFramebuffers(uint32_t total, uint32_t* buffers);
  static void deleteFramebuffers(uint32_t total, const uint32_t* buffers);
  static uint32_t genFramebuffer();
  static void deleteFramebuffer(uint32_t bufferId);
  static void framebufferTexture2D(uint32_t textureId);
  static void framebufferRenderbuffer(uint32_t bufferId);
  static void checkFrameBuffer();
  static void bindFramebuffer(uint32_t frameBufferId);

public:
  static void enableVertexAttribArray(uint32_t attrId);
  static void vertexAttribPointer(uint32_t attrId, uint32_t rowSize, uint32_t stride, uint32_t rowIndex);
  static void enableVertexAttribDivisor(uint32_t attrId);
  static void bufferData(const void* data, uint32_t dataSize, bool dynamic);
  static void drawArrays(Primitives primitive, uint32_t primitiveStart, uint32_t primitiveCount);
  static void drawArraysInstanced(Primitives primitive, uint32_t primitiveStart, uint32_t primitiveCount, uint32_t instanceCount);

public:
  static void genRenderbuffers(uint32_t total, uint32_t* buffers);
  static void deleteRenderbuffers(uint32_t total, const uint32_t* buffers);
  static void bindRenderbuffer(uint32_t renderBufferId);
  static void renderbufferStorage(uint32_t width, uint32_t height);

public:
  static void printShaderInfo(uint32_t shader);
  static void printProgramInfo(uint32_t program);
  static uint32_t loadVertexShader(const std::string& source);
  static uint32_t loadFragmentShader(const std::string& source);
  static uint32_t createProgram();
  static void deleteProgram(uint32_t programId);
  static bool linkProgram(uint32_t programId, uint32_t vertexShader, uint32_t fragmentShader);
  static int getAttribLocation(uint32_t programId, const char* name);
  static int getUniformLocation(uint32_t programId, const char* name);
  static void useProgram(uint32_t programId);
  static void setUniform(int location, float value);
  static void setUniform(int location, float x, float y, float z);
  static void setUniform(int location, float x, float y, float z, float w);
  static void setUniform(int location, const glm::mat4& matrix);

public:
  static void genTextures(uint32_t total, uint32_t* buffers);
  static void deleteTextures(uint32_t total, const uint32_t* buffers);
  static uint32_t genTexture();
  static void deleteTexture(uint32_t textureId);
  static void bindTexture(uint32_t textureId);
  static void uploadPixels(uint32_t width, uint32_t height, const void* pixels);
  static void setTextureAsRepeat(bool repeat);
  static void setTextureAsPixelated();
  static void setTextureAsSmoothed(bool generateMipMap = false);

public:
  static void genVertexArrays(uint32_t total, uint32_t* buffers);
  static void deleteVertexArrays(uint32_t total, const uint32_t* buffers);
  static void bindVertexArray(uint32_t vaoId);

public:
  static void genBuffers(uint32_t total, uint32_t* buffers);
  static void deleteBuffers(uint32_t total, const uint32_t* buffers);
  static void bindBuffer(uint32_t vboId);

public:
  static std::string getVersion();
  static void setViewport(int x, int y, uint32_t width, uint32_t height);
  static void setScissor(int x, int y, uint32_t width, uint32_t height);

  enum class DepthFuncs
  {
    never,
    less,
    equal,
    lessAndEqual,
    greater,
    notEqual,
    greaterAndEqual,
    always,
  };
  static void setDepthFunc(DepthFuncs func);

  enum class BlendFuncs
  {
    srcAlpha,
    oneMinuxSrcAlpha,
  };
  static void setBlendFunc(BlendFuncs sfactor, BlendFuncs dfactor);


  enum class States
  {
    cullFace,
    depthTest,
    blend,
    scissorTest,
  };
  static void enable(States state);
  static void disable(States state);

  enum class BackFaceCullingDirection
  {
    clockWise,
    counterClockWise,
  };
  static void setBackFaceCullingDirection(BackFaceCullingDirection direction);


  enum class Buffers : int
  {
    color = 1 << 0,
    depth = 1 << 1,
  };

  static void clear(int mask);
  static void clearColor(float r, float g, float b, float a);

  // static void polygonModeAsLine();
  // static void polygonModeAsFill();

};
