#include "RockDensityGenerator.hpp"
#include "TerrainBlock.hpp"
#include "PerlinNoiseGenerator.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace Synergy;
using namespace glm;


//---------------------------------------------------------------------------------------
RockDensityGenerator::RockDensityGenerator() {
	glGenVertexArrays(1, &junkVao);
	glGenFramebuffers(1, &framebuffer);

	setupShaderProgram();

	uvec3 noiseTextureDimensions(16,16,16);
	perlinNoiseGenerator = new PerlinNoiseGenerator(noiseTextureDimensions);
	perlinNoiseGenerator->getNoiseTexture(noiseTexture);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
RockDensityGenerator::~RockDensityGenerator() {
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteVertexArrays(1, &junkVao);
}

//---------------------------------------------------------------------------------------
void RockDensityGenerator::generateRockDensity(
		TerrainBlock & block
) {
	shader_computeRockDensity.setUniform("wsBlockMinVertPos", block.wsMinVertexPos);
	computeRockDensity();


}

//---------------------------------------------------------------------------------------
void RockDensityGenerator::setupShaderProgram() {
	shader_computeRockDensity.generateProgramObject();
	shader_computeRockDensity.attachVertexShader("shaders/VolumeFillingTriangles.vs");
	shader_computeRockDensity.attachGeometryShader("shaders/VolumeFillingTriangles.gs");
	shader_computeRockDensity.attachFragmentShader("shaders/ComputeRockDensity.fs");
	shader_computeRockDensity.link();

	GLint texUnitOffset = 0;
	shader_computeRockDensity.setUniform("noiseTexture", texUnitOffset);

	mat3 rotMat = mat3(glm::rotate(mat4(), 0.02f, vec3(0.1,0.1,-0.1)));
	shader_computeRockDensity.setUniform("rotMat", rotMat);
}

//---------------------------------------------------------------------------------------
void RockDensityGenerator::computeRockDensity() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//-- Disable depth testing and writing to depth buffer:
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	// Set viewport size to match destination texture, the texture to be written to.
	GLint prevViewportData[4];
	glGetIntegerv(GL_VIEWPORT, prevViewportData);
	glViewport(0, 0, densityTexture.width(), densityTexture.height());


	glActiveTexture(GL_TEXTURE0);
	noiseTexture.bind();


	// Bind a junk VAO so driver doesn't complain.
	glBindVertexArray(junkVao);

	GLsizei numInstances = densityTexture.depth();

	shader_computeRockDensity.enable();
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, numInstances);
	shader_computeRockDensity.disable();

	//-- Restore defaults:
	noiseTexture.unbind();
	glViewport(prevViewportData[0],prevViewportData[1],
			prevViewportData[2],prevViewportData[3]);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void RockDensityGenerator::setTextureRenderTarget(
		const Synergy::Texture3D & texture
) {
	densityTexture = texture;

	shader_computeRockDensity.setUniform("textureDepth", float(texture.depth()));

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.name(), 0);

	CHECK_FRAMEBUFFER_COMPLETENESS;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//---------------------------------------------------------------------------------------
// TODO Dustin - Remove this eventually once rock density shader is working.
// Fallback to basic rock dentiy calculations
void RockDensityGenerator::generateBasicRock(TerrainBlock &block) {
	const Texture3D & densityTexture = *(block.densityTexture);

	const int width = densityTexture.width();
	const int height = densityTexture.height();
	const int depth = densityTexture.depth();

	float32 * data = new float32[depth * height * width];

	int index;
	float value;
	for(int k(0); k < depth; ++k) {
		for(int j(0); j < height; ++j) {
			for(int i(0); i < width; ++i) {

				index = k * (height * width) + (j * width) + i;

				value = -k;
				value += j*0.98f;   // Slope below 1 creates a plane through voxel.
//				value += j;  // Slope of 1 or great causes plane to distort.

				data[index] = value;
			}
		}
	}

	//////////////////////////
	// Diagonal slope in x-dir
	// Correct
	data[0] = 0.0f;
	data[1] = 1.0f;
	data[2] = 0.0f;
	data[3] = 1.0f;

	data[4] = -1.0f;
	data[5] = 0.0f;
	data[6] = -1.0f;
	data[7] = 0.0f;
	//////////////////////////


	//////////////////////////
	// Diagonal slope in y-dir, Slope == 1
	// Incorrect triangles
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 1.0f;
	data[3] = 1.0f;

	data[4] = -1.0f;
	data[5] = -1.0f;
	data[6] = 0.0f;
	data[7] = 0.0f;
	//////////////////////////

	//////////////////////////
	// Diagonal slope in y-dir.  Slope < 1
	// Correct
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 1.0f;
	data[3] = 1.0f;

	data[4] = -1.0f;
	data[5] = -1.0f;
	data[6] = -0.01f;
	data[7] = -0.01f;
	//////////////////////////


	//////////////////////////
	// Diagonal slope in y-dir.  Slope > 1
	// Incorrect Triangles
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 1.0f;
	data[3] = 1.0f;

	data[4] = -1.0f;
	data[5] = -1.0f;
	data[6] = 0.3f;
	data[7] = 0.3f;
	//////////////////////////


	densityTexture.bind();
	// Copy texture into data's memory:
	glTexSubImage3D(densityTexture.type, 0, 0, 0, 0, width,
			height, depth, GL_RED, GL_FLOAT, data);
	densityTexture.unbind();


	delete [] data;

}
