#include "RockDensityGenerator.hpp"
#include "TerrainBlock.hpp"

using namespace Synergy;



//---------------------------------------------------------------------------------------
RockDensityGenerator::RockDensityGenerator() {
	glGenVertexArrays(1, &junkVao);
	glGenFramebuffers(1, &framebuffer);

	setupShaderProgram();

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

	GLsizei numInstances = densityTexture.depth();

	// Bind a junk VAO so driver doesn't complain.
	glBindVertexArray(junkVao);

	shader_computeRockDensity.enable();
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, numInstances);
	shader_computeRockDensity.disable();

	//-- Restore defaults:
	densityTexture.unbind();
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

	float isoValue = 0;

	for(int k(0); k < depth; ++k) {
		for(int j(0); j < height; ++j) {
			for(int i(0); i < width; ++i) {
				// Set all values below isosurface isoValue
				data[k * (height * width) + (j * width) + i] = isoValue - 1.0f;
			}
		}
	}

//	data[(0 * height * width) + (0 * width) + 0] = isoValue + 1;  // Vertex 0
//    data[(0 * height * width) + (0 * width) + 1] = isoValue + 1;  // Vertex 1
	data[(0 * height * width) + (0 * width) + 2] = isoValue + 1;  // Vertex 2

	data[(0 * height * width) + (1 * width) + 0] = isoValue + 1;  // Vertex 3
	data[(0 * height * width) + (1 * width) + 1] = isoValue + 1;  // Vertex 4
	data[(0 * height * width) + (1 * width) + 2] = isoValue + 1;  // Vertex 5

//    data[(0 * height * width) + (2 * width) + 0] = isoValue + 1;  // Vertex 6
	data[(0 * height * width) + (2 * width) + 1] = isoValue + 1;  // Vertex 7
	data[(0 * height * width) + (2 * width) + 2] = isoValue + 1;  // Vertex 8

//    data[(1 * height * width) + (0 * width) + 0] = isoValue + 1; //Vertex 9
//    data[(1 * height * width) + (0 * width) + 1] = isoValue + 1; //Vertex 10
	data[(1 * height * width) + (0 * width) + 2] = isoValue + 1; //Vertex 11

//    data[(1 * height * width) + (1 * width) + 0] = isoValue + 1; //Vertex 12
	data[(1 * height * width) + (1 * width) + 1] = isoValue + 1; //Vertex 13
//    data[(1 * height * width) + (1 * width) + 2] = isoValue + 1; //Vertex 14

//    data[(1 * height * width) + (2 * width) + 0] = isoValue + 1; //Vertex 15
	data[(1 * height * width) + (2 * width) + 1] = isoValue + 1; //Vertex 16
//    data[(1 * height * width) + (2 * width) + 2] = isoValue + 1; //Vertex 17

	data[(2 * height * width) + (0 * width) + 0] = isoValue + 1; //Vertex 18
//    data[(2 * height * width) + (0 * width) + 1] = isoValue + 1; //Vertex 19
//    data[(2 * height * width) + (0 * width) + 2] = isoValue + 1; //Vertex 20

	data[(2 * height * width) + (1 * width) + 0] = isoValue + 1; //Vertex 21
//    data[(2 * height * width) + (1 * width) + 1] = isoValue + 1; //Vertex 22
	data[(2 * height * width) + (1 * width) + 2] = isoValue + 1; //Vertex 23

//    data[(2 * height * width) + (2 * width) + 0] = isoValue + 1; //Vertex 24
//    data[(2 * height * width) + (2 * width) + 1] = isoValue + 1; //Vertex 25
//    data[(2 * height * width) + (2 * width) + 2] = isoValue + 1; //Vertex 26

	densityTexture.bind();
	// Copy texture into data's memory:
	glTexSubImage3D(densityTexture.type, 0, 0, 0, 0, width,
			height, depth, GL_RED, GL_FLOAT, data);
	densityTexture.unbind();


	delete [] data;

}
