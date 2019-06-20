
#include "Data.hpp"

#include "graphic/wrappers/Shader.hpp"

#include <memory> // <= make_unique

void	Data::initialiseShaders()
{

	std::array<Shader::t_def::t_uniformBlock, e_uboTypes::eCount> uboBindings;
	uboBindings[e_uboTypes::eMatrices] = { "u_Matrices", 0 };
	uboBindings[e_uboTypes::eColors] = { "u_Colors", 1 };
	uboBindings[e_uboTypes::eBlending] = { "u_Blending", 2 };
	uboBindings[e_uboTypes::eAnimation] = { "u_Animation", 3 };

	{

		{ // create the basic shader

			Shader::t_def	def;
			def.filenames.vertex = "assets/shaders/basic.glsl.vert.c";
			def.filenames.fragment = "assets/shaders/basic.glsl.frag.c";
			def.attributes = { "a_position", "a_color" };
			def.uniformBlocks = {
				uboBindings[e_uboTypes::eMatrices],
			};

			graphic.shaders.basic = new Shader(def);
		}

		{ // create the instanced shader

			Shader::t_def	def;
			def.filenames.vertex = "assets/shaders/instanced.glsl.vert.c";
			def.filenames.fragment = "assets/shaders/instanced.glsl.frag.c";
			def.attributes = { "a_position", "a_transform" };
			def.uniformBlocks = {
				uboBindings[e_uboTypes::eMatrices],
				uboBindings[e_uboTypes::eColors],
			};

			graphic.shaders.instanced = new Shader(def);
		}

		{ // create the mono color shader

			Shader::t_def	def;

			def.filenames.vertex = "assets/shaders/monoColor.glsl.vert.c";
			def.filenames.fragment = "assets/shaders/monoColor.glsl.frag.c";
			def.attributes = { "a_position" };
			def.uniformBlocks = {
				uboBindings[e_uboTypes::eMatrices],
				uboBindings[e_uboTypes::eColors],
			};

			graphic.shaders.monoColor = new Shader(def);
		}

		{ // create the animated circuit shader

			Shader::t_def	def;

			def.filenames.vertex = "assets/shaders/animatedCircuit.glsl.vert.c";
			def.filenames.fragment = "assets/shaders/animatedCircuit.glsl.frag.c";
			def.attributes = { "a_position", "a_color", "a_normal", "a_index" };
			def.uniformBlocks = {
				uboBindings[e_uboTypes::eMatrices],
				uboBindings[e_uboTypes::eBlending],
				uboBindings[e_uboTypes::eAnimation],
			};

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
			def.uniforms = { "u_texture" };
			def.uniformBlocks = {
				uboBindings[e_uboTypes::eMatrices],
			};

			graphic.shaders.hudText = new Shader(def);
		}

	}

	{
		bool pixelated = true;
		graphic.textures.textFont.load("assets/textures/ascii_font.png", pixelated);
	}

	{ // create the uniform buffer object

		auto& ubo = graphic.ubo;

		ubo.allocate(uboBindings.size());
		for (unsigned int ii = 0; ii < uboBindings.size(); ++ii)
		{
			ubo.setBoundLayout(ii, uboBindings[ii].layout);
			ubo.bindBase(ii);
		}

		GLint offsetValue = UniformBufferObject::getOffsetAlignment();
		GLint baseAlignment = sizeof(float);

		//
		// instanced / monocolor
		//

		{
			int bufPageSize = offsetValue * baseAlignment;

			int rawBufSize = bufPageSize * 4;
			auto scopedBuffer = std::make_unique<char[]>(rawBufSize);

			char* rawBufferPtr = scopedBuffer.get();

			std::memset(rawBufferPtr, 0, rawBufSize);

			glm::vec4	chassisColor = {1.0f, 0.0f, 0.0f, 1.0f};
			glm::vec4	wheelsColor = {1.0f, 1.0f, 0.0f, 1.0f};
			glm::vec4	circuitColor = {0.6f, 0.6f, 0.6f, 1.0f};
			glm::vec4	trailsColor = {1.0f, 1.0f, 1.0f, 1.0f};

			std::memcpy(rawBufferPtr + 0 * bufPageSize, &chassisColor.x, sizeof(chassisColor.x) * 4);
			std::memcpy(rawBufferPtr + 1 * bufPageSize, &wheelsColor.x, sizeof(wheelsColor.x) * 4);
			std::memcpy(rawBufferPtr + 2 * bufPageSize, &circuitColor.x, sizeof(circuitColor.x) * 4);
			std::memcpy(rawBufferPtr + 3 * bufPageSize, &trailsColor.x, sizeof(trailsColor.x) * 4);

			ubo.updateBuffer(e_uboTypes::eColors, rawBufferPtr, rawBufSize, true);
		}

		//
		// animated
		//

		{
			int bufPageSize = offsetValue;

			int rawBufSize = bufPageSize * 2;
			auto scopedBuffer = std::make_unique<char[]>(rawBufSize);

			char* rawBufferPtr = scopedBuffer.get();

			std::memset(rawBufferPtr, 0, rawBufSize);

			const float groundAlpha = 0.8f;
			const float wallsAlpha = 0.2f;

			std::memcpy(rawBufferPtr + 0 * bufPageSize, &groundAlpha, sizeof(groundAlpha));
			std::memcpy(rawBufferPtr + 1 * bufPageSize, &wallsAlpha, sizeof(wallsAlpha));

			ubo.updateBuffer(e_uboTypes::eBlending, rawBufferPtr, rawBufSize, true);
		}

	}

}
