
#include "Data.hpp"

#include "graphic/wrappers/Shader.hpp"

#include <memory> // <= make_unique

void	Data::initialiseShaders()
{

	{

		{ // create the stackRenderer shader

			Shader::t_def	def;
			def.filenames.vertex = "assets/shaders/stackRenderer.glsl.vert.c";
			def.filenames.fragment = "assets/shaders/stackRenderer.glsl.frag.c";
			def.attributes = { "a_position", "a_color" };
			def.uniforms = { "u_composedMatrix" };

			graphic.shaders.stackRenderer = new Shader(def);
		}

		{ // create the instanced shader

			Shader::t_def	def;
			def.filenames.vertex = "assets/shaders/instanced.glsl.vert.c";
			def.filenames.fragment = "assets/shaders/instanced.glsl.frag.c";
			// def.attributes = { "a_position", "a_transform" };
			// def.uniforms = { "u_composedMatrix", "u_color" };
			def.attributes = { "a_position", "a_transform", "a_color" };
			def.uniforms = { "u_composedMatrix" };

			graphic.shaders.instanced = new Shader(def);
		}

		{ // create the mono color shader

			Shader::t_def	def;

			def.filenames.vertex = "assets/shaders/monoColor.glsl.vert.c";
			def.filenames.fragment = "assets/shaders/monoColor.glsl.frag.c";
			def.attributes = { "a_position" };
			def.uniforms = { "u_composedMatrix", "u_color" };

			graphic.shaders.monoColor = new Shader(def);
		}

		{ // create the animated circuit shader

			Shader::t_def	def;

			def.filenames.vertex = "assets/shaders/animatedCircuit.glsl.vert.c";
			def.filenames.fragment = "assets/shaders/animatedCircuit.glsl.frag.c";
			def.attributes = { "a_position", "a_color", "a_normal", "a_index" };
			def.uniforms = { "u_composedMatrix", "u_alpha", "u_lowerLimit", "u_upperLimit" };

			graphic.shaders.animatedCircuit = new Shader(def);
		}

		{ // create the hud text shader

			Shader::t_def	def;

			def.filenames.vertex = "assets/shaders/hudText.glsl.vert.c";
			def.filenames.fragment = "assets/shaders/hudText.glsl.frag.c";
			def.attributes = {
				"a_position", "a_texCoord",
				"a_offsetPosition", "a_offsetTexCoord", "a_offsetScale"
			};
			def.uniforms = { "u_composedMatrix", "u_texture", };

			graphic.shaders.hudText = new Shader(def);
		}

	}

	{
		bool pixelated = true;
		graphic.textures.textFont.load("assets/textures/ascii_font.png", pixelated);
	}
}
