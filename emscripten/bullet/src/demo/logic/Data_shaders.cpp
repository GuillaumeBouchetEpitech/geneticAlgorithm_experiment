
#include "Data.hpp"

#include "graphic/wrappers/Shader.hpp"

#include <memory> // <= make_unique

void Data::initialiseShaders()
{

    { // shaders

        { // create the stackRenderer shader

            Shader::t_def def;
            def.filenames.vertex = "assets/shaders/stackRenderer.glsl.vert.c";
            def.filenames.fragment = "assets/shaders/stackRenderer.glsl.frag.c";
            def.attributes = { "a_position", "a_color" };
            def.uniforms = { "u_composedMatrix" };

            graphic.shaders.stackRenderer = new Shader(def);
        }

        { // create the wireframes shader

            Shader::t_def def;

            def.filenames.vertex = "assets/shaders/wireframes.glsl.vert.c";
            def.filenames.fragment = "assets/shaders/wireframes.glsl.frag.c";
            def.attributes = { "a_position" };
            def.uniforms = { "u_composedMatrix", "u_color" };

            graphic.shaders.wireframes = new Shader(def);
        }

        { // create the animated circuit shader

            Shader::t_def def;

            def.filenames.vertex = "assets/shaders/animatedCircuit.glsl.vert.c";
            def.filenames.fragment = "assets/shaders/animatedCircuit.glsl.frag.c";
            def.attributes = { "a_position", "a_color", "a_normal", "a_index" };
            def.uniforms = { "u_composedMatrix", "u_alpha", "u_lowerLimit", "u_upperLimit" };

            graphic.shaders.animatedCircuit = new Shader(def);
        }

        { // create the hud text shader

            Shader::t_def def;

            def.filenames.vertex = "assets/shaders/hudText.glsl.vert.c";
            def.filenames.fragment = "assets/shaders/hudText.glsl.frag.c";
            def.attributes = {
                "a_position", "a_texCoord",
                "a_offsetPosition", "a_offsetTexCoord", "a_offsetScale"
            };
            def.uniforms = { "u_composedMatrix", "u_texture", };

            graphic.shaders.hudText = new Shader(def);
        }

        { // particles

            Shader::t_def def;
            def.filenames.vertex = "assets/shaders/particles.glsl.vert.c";
            def.filenames.fragment = "assets/shaders/particles.glsl.frag.c";
            def.attributes = {
                "a_position",
                "a_offsetPosition", "a_offsetScale", "a_offsetColor"
            };
            def.uniforms = { "u_composedMatrix" };

            graphic.shaders.particles = new Shader(def);
        }

        { // model (chassis + wheels)

            Shader::t_def def;
            def.filenames.vertex = "assets/shaders/model.glsl.vert.c";
            def.filenames.fragment = "assets/shaders/model.glsl.frag.c";
            def.attributes = { "a_position", "a_color", "a_offsetTransform", "a_offsetColor" };
            def.uniforms = { "u_composedMatrix" };

            graphic.shaders.model = new Shader(def);
        }

    } // shaders

    { // textures

        // font textre
        bool pixelated = true;
        graphic.textures.textFont.load("assets/textures/ascii_font.png", pixelated);

    } // textures

}
