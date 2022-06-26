
#include "Data.hpp"

#include "graphic/wrappers/ShaderProgramBuilder.hpp"

void Data::initialiseShaders()
{
    ShaderProgramBuilder shaderProgramBuilder;

    graphic.shaders.stackRenderer = shaderProgramBuilder
        .reset()
        .setVertexFilename("assets/shaders/stackRenderer.vert")
        .setFragmentFilename("assets/shaders/stackRenderer.frag")
        .addAttribute("a_position")
        .addAttribute("a_color")
        .addUniform("u_composedMatrix")
        .build();

    graphic.shaders.wireframes = shaderProgramBuilder
        .reset()
        .setVertexFilename("assets/shaders/wireframes.vert")
        .setFragmentFilename("assets/shaders/wireframes.frag")
        .addAttribute("a_position")
        .addUniform("u_composedMatrix")
        .addUniform("u_color")
        .build();

    graphic.shaders.animatedCircuit = shaderProgramBuilder
        .reset()
        .setVertexFilename("assets/shaders/animatedCircuit.vert")
        .setFragmentFilename("assets/shaders/animatedCircuit.frag")
        .addAttribute("a_position")
        .addAttribute("a_color")
        .addAttribute("a_normal")
        .addAttribute("a_index")
        .addUniform("u_composedMatrix")
        .addUniform("u_alpha")
        .addUniform("u_lowerLimit")
        .addUniform("u_upperLimit")
        .build();

    graphic.shaders.hudText = shaderProgramBuilder
        .reset()
        .setVertexFilename("assets/shaders/hudText.vert")
        .setFragmentFilename("assets/shaders/hudText.frag")
        .addAttribute("a_position")
        .addAttribute("a_texCoord")
        .addAttribute("a_offsetPosition")
        .addAttribute("a_offsetTexCoord")
        .addAttribute("a_offsetColor")
        .addAttribute("a_offsetScale")
        .addUniform("u_composedMatrix")
        .addUniform("u_texture")
        .build();

    graphic.shaders.particles = shaderProgramBuilder
        .reset()
        .setVertexFilename("assets/shaders/particles.vert")
        .setFragmentFilename("assets/shaders/particles.frag")
        .addAttribute("a_position")
        .addAttribute("a_offsetPosition")
        .addAttribute("a_offsetScale")
        .addAttribute("a_offsetColor")
        .addUniform("u_composedMatrix")
        .build();

    graphic.shaders.model = shaderProgramBuilder
        .reset()
        .setVertexFilename("assets/shaders/model.vert")
        .setFragmentFilename("assets/shaders/model.frag")
        .addAttribute("a_position")
        .addAttribute("a_color")
        .addAttribute("a_offsetTransform")
        .addAttribute("a_offsetColor")
        .addUniform("u_composedMatrix")
        .build();

    graphic.shaders.simpleTexture = shaderProgramBuilder
        .reset()
        .setVertexFilename("assets/shaders/simpleTexture.vert")
        .setFragmentFilename("assets/shaders/simpleTexture.frag")
        .addAttribute("a_position")
        .addAttribute("a_texCoord")
        .addUniform("u_composedMatrix")
        .addUniform("u_texture")
        .build();
}
