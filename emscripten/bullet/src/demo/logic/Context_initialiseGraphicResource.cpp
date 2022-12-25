
#include "Context.hpp"

#include "graphicIds.hpp"

#include "framework/system/asValue.hpp"

#include "framework/graphic/GeometryBuilder.hpp"
#include "framework/graphic/ShaderProgramBuilder.hpp"

void Context::initialiseGraphicResource() {
  ShaderProgramBuilder shaderProgramBuilder;
  GeometryBuilder geometryBuilder;

  auto& rManager = graphic.resourceManager;

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/stackRenderer.vert")
      .setFragmentFilename("assets/shaders/scene/stackRenderer.frag")
      .addAttribute("a_position")
      .addAttribute("a_color")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(asValue(ShaderIds::stackRendererScene),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_color", Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      asValue(GeometryIds::stackRendererWireframesScene),
      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(Geometry::PrimitiveType::triangles);

    rManager.createGeometryDefinition(
      asValue(GeometryIds::stackRendererTrianglesScene),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/hud/stackRenderer.vert")
      .setFragmentFilename("assets/shaders/hud/stackRenderer.frag")
      .addAttribute("a_position")
      .addAttribute("a_color")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(asValue(ShaderIds::stackRendererHud),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_color", Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      asValue(GeometryIds::stackRendererWireframesHud),
      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(Geometry::PrimitiveType::triangles);

    rManager.createGeometryDefinition(
      asValue(GeometryIds::stackRendererTrianglesHud),
      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/particles.vert")
      .setFragmentFilename("assets/shaders/scene/particles.frag")
      .addAttribute("a_position")
      .addAttribute("a_offsetPosition")
      .addAttribute("a_offsetScale")
      .addAttribute("a_offsetColor")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(asValue(ShaderIds::particles),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offsetPosition", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offsetScale", Geometry::AttrType::Float)
      .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec3f);

    rManager.createGeometryDefinition(asValue(GeometryIds::particles),
                                      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/backGroundTorus.vert")
      .setFragmentFilename("assets/shaders/scene/backGroundTorus.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture")
      .addUniform("u_animationCoef");

    auto shader = rManager.createShader(asValue(ShaderIds::backGroundTorus),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f)
      ;

    rManager.createGeometryDefinition(asValue(GeometryIds::backGroundTorus),
                                      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/simpleTexture.vert")
      .setFragmentFilename("assets/shaders/scene/simpleTexture.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture");

    auto shader = rManager.createShader(asValue(ShaderIds::simpleTexture),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f);

    rManager.createGeometryDefinition(asValue(GeometryIds::simpleTexture),
                                      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/simpleTextureLit.vert")
      .setFragmentFilename("assets/shaders/scene/simpleTextureLit.frag")
      .addAttribute("a_position")
      .addAttribute("a_normal")
      .addAttribute("a_texCoord")
      .addUniform("u_projectionMatrix")
      .addUniform("u_modelViewMatrix")
      .addUniform("u_texture");

    auto shader = rManager.createShader(asValue(ShaderIds::simpleTextureLit),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_normal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f);

    rManager.createGeometryDefinition(asValue(GeometryIds::simpleTextureLit),
                                      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/wireframes.vert")
      .setFragmentFilename("assets/shaders/scene/wireframes.frag")
      .addAttribute("a_position")
      .addUniform("u_composedMatrix")
      .addUniform("u_color");

    auto shader = rManager.createShader(asValue(ShaderIds::wireframes),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec3f);

    rManager.createGeometryDefinition(asValue(GeometryIds::wireframes),
                                      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(Geometry::PrimitiveType::line_strip);

    rManager.createGeometryDefinition(asValue(GeometryIds::wireframesLineStrip),
                                      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/animatedCircuit.vert")
      .setFragmentFilename("assets/shaders/scene/animatedCircuit.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addAttribute("a_vertex_animatedNormal")
      .addAttribute("a_vertex_index")
      .addUniform("u_composedMatrix")
      .addUniform("u_alpha")
      .addUniform("u_lowerLimit")
      .addUniform("u_upperLimit");

    auto shader = rManager.createShader(asValue(ShaderIds::animatedCircuit),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_animatedNormal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_index", Geometry::AttrType::Float);

    rManager.createGeometryDefinition(asValue(GeometryIds::animatedCircuit),
                                      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/animatedCircuitLit.vert")
      .setFragmentFilename("assets/shaders/scene/animatedCircuitLit.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addAttribute("a_vertex_normal")
      .addAttribute("a_vertex_animatedNormal")
      .addAttribute("a_vertex_index")
      .addUniform("u_projectionMatrix")
      .addUniform("u_modelViewMatrix")
      .addUniform("u_alpha")
      .addUniform("u_lowerLimit")
      .addUniform("u_upperLimit");

    auto shader = rManager.createShader(asValue(ShaderIds::animatedCircuitLit),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_animatedNormal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_index", Geometry::AttrType::Float);

    rManager.createGeometryDefinition(asValue(GeometryIds::animatedCircuitLit),
                                      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {

    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/models.vert")
      .setFragmentFilename("assets/shaders/scene/models.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addAttribute("a_offset_transform")
      .addAttribute("a_offset_color")
      .addAttribute("a_offset_outlineColor")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(asValue(ShaderIds::models),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offset_transform", Geometry::AttrType::Mat4f)
      .addVboAttribute("a_offset_color", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_outlineColor", Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(asValue(GeometryIds::models),
                                      geometryBuilder.getDefinition(), true);
  }

  {

    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/scene/modelsLit.vert")
      .setFragmentFilename("assets/shaders/scene/modelsLit.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addAttribute("a_vertex_normal")
      .addAttribute("a_offset_transform")
      .addAttribute("a_offset_color")
      .addAttribute("a_offset_outlineColor")
      .addUniform("u_projectionMatrix")
      .addUniform("u_modelViewMatrix");

    auto shader = rManager.createShader(asValue(ShaderIds::modelsLit),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offset_transform", Geometry::AttrType::Mat4f)
      .addVboAttribute("a_offset_color", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_outlineColor", Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(asValue(GeometryIds::modelsLit),
                                      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/hud/hudText.vert")
      .setFragmentFilename("assets/shaders/hud/hudText.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addAttribute("a_offsetPosition")
      .addAttribute("a_offsetTexCoord")
      .addAttribute("a_offsetColor")
      .addAttribute("a_offsetScale")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture");

    auto shader = rManager.createShader(asValue(ShaderIds::hudText),
                                        shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", Geometry::AttrType::Vec2f)
      .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offsetPosition", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offsetTexCoord", Geometry::AttrType::Vec2f)
      .addVboAttribute("a_offsetColor", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offsetScale", Geometry::AttrType::Float);

    rManager.createGeometryDefinition(asValue(GeometryIds::hudText),
                                      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {

    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/hud/postProcess.glsl.vert")
      .setFragmentFilename("assets/shaders/hud/postProcess.glsl.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_colorTexture")
      .addUniform("u_outlineTexture")
      .addUniform("u_invResolution");

    auto shader = rManager.createShader(asValue(ShaderIds::postProcess),
                                        shaderProgramBuilder.getDefinition());

    // geometryBuilder.reset()
    //   .setShader(*shader)
    //   .setPrimitiveType(Geometry::PrimitiveType::triangles)
    //   .addVbo()
    //   .addVboAttribute("a_position", Geometry::AttrType::Vec3f)
    //   .addVboAttribute("a_texCoord", Geometry::AttrType::Vec2f);

    // rManager.createGeometryDefinition(asValue(GeometryIds::postProcess),
    //                                   geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  { // font

    rManager.createTexture(0, "assets/textures/ascii_font.png",
                           Texture::Quality::pixelated,
                           Texture::Pattern::clamped);

  } // font
}
