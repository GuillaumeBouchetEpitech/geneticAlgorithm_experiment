
#include "Context.hpp"

#include "graphicIds.hpp"

#include "framework/asValue.hpp"

#include "framework/graphic/ResourceManager.hpp"

#include "framework/graphic/ShaderProgramBuilder.hpp"

void Context::initialiseGraphicResource() {
  ShaderProgramBuilder shaderProgramBuilder;

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/stackRenderer.vert")
      .setFragmentFilename("assets/shaders/stackRenderer.frag")
      .addAttribute("a_position")
      .addAttribute("a_color")
      .addUniform("u_composedMatrix");

    graphic.resourceManager.createShader(asValue(Shaders::stackRenderer),
                                         shaderProgramBuilder.getDefinition());
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/particles.vert")
      .setFragmentFilename("assets/shaders/particles.frag")
      .addAttribute("a_position")
      .addAttribute("a_offsetPosition")
      .addAttribute("a_offsetScale")
      .addAttribute("a_offsetColor")
      .addUniform("u_composedMatrix");

    graphic.resourceManager.createShader(asValue(Shaders::particles),
                                         shaderProgramBuilder.getDefinition());
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/simpleTexture.vert")
      .setFragmentFilename("assets/shaders/simpleTexture.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture");

    graphic.resourceManager.createShader(asValue(Shaders::simpleTexture),
                                         shaderProgramBuilder.getDefinition());
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/litTexture.vert")
      .setFragmentFilename("assets/shaders/litTexture.frag")
      .addAttribute("a_position")
      .addAttribute("a_normal")
      .addAttribute("a_texCoord")
      .addUniform("u_projectionMatrix")
      .addUniform("u_modelViewMatrix")
      .addUniform("u_texture");

    graphic.resourceManager.createShader(asValue(Shaders::litTexture),
                                         shaderProgramBuilder.getDefinition());
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/hudText.vert")
      .setFragmentFilename("assets/shaders/hudText.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addAttribute("a_offsetPosition")
      .addAttribute("a_offsetTexCoord")
      .addAttribute("a_offsetColor")
      .addAttribute("a_offsetScale")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture");

    graphic.resourceManager.createShader(asValue(Shaders::hudText),
                                         shaderProgramBuilder.getDefinition());
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/wireframes.vert")
      .setFragmentFilename("assets/shaders/wireframes.frag")
      .addAttribute("a_position")
      .addUniform("u_composedMatrix")
      .addUniform("u_color");

    graphic.resourceManager.createShader(asValue(Shaders::wireframes),
                                         shaderProgramBuilder.getDefinition());
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/animatedCircuit.vert")
      .setFragmentFilename("assets/shaders/animatedCircuit.frag")
      .addAttribute("a_position")
      .addAttribute("a_color")
      .addAttribute("a_animated_normal")
      .addAttribute("a_index")
      .addUniform("u_composedMatrix")
      .addUniform("u_alpha")
      .addUniform("u_lowerLimit")
      .addUniform("u_upperLimit");

    graphic.resourceManager.createShader(asValue(Shaders::animatedCircuit),
                                         shaderProgramBuilder.getDefinition());
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/animatedCircuitLit.vert")
      .setFragmentFilename("assets/shaders/animatedCircuitLit.frag")
      .addAttribute("a_position")
      .addAttribute("a_color")
      .addAttribute("a_normal")
      .addAttribute("a_animated_normal")
      .addAttribute("a_index")
      .addUniform("u_projectionMatrix")
      .addUniform("u_modelViewMatrix")
      .addUniform("u_alpha")
      .addUniform("u_lowerLimit")
      .addUniform("u_upperLimit");

    graphic.resourceManager.createShader(asValue(Shaders::animatedCircuitLit),
                                         shaderProgramBuilder.getDefinition());
  }

  //
  //
  //

  {
    shaderProgramBuilder.reset()
      .setVertexFilename("assets/shaders/model.vert")
      .setFragmentFilename("assets/shaders/model.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addAttribute("a_vertex_normal")
      .addAttribute("a_offset_transform")
      .addAttribute("a_offset_color")
      .addUniform("u_projectionMatrix")
      .addUniform("u_modelViewMatrix");

    graphic.resourceManager.createShader(asValue(Shaders::models),
                                         shaderProgramBuilder.getDefinition());
  }

  { // font

    graphic.resourceManager.createTexture(0, "assets/textures/ascii_font.png",
                                          Texture::Quality::pixelated,
                                          Texture::Pattern::clamped);

  } // font
}
