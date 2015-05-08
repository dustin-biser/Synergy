#include "LightingOven.hpp"

using namespace Synergy;

//---------------------------------------------------------------------------------------
LightingOven::LightingOven(
		const glm::uvec3 & densityTextureDimensions,
		const glm::uvec3 & normalAmboTextureDimensions
) {
	setupShaderProgram();
	setShaderUniforms(densityTextureDimensions, normalAmboTextureDimensions);

	glGenVertexArrays(1, &junkVao);
	glGenFramebuffers(1, &framebuffer);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
LightingOven::~LightingOven() {
	glDeleteVertexArrays(1, &junkVao);
	glDeleteFramebuffers(1, &framebuffer);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void LightingOven::setupShaderProgram() {
	shader_computeNormals.generateProgramObject();
	shader_computeNormals.attachVertexShader("shaders/VolumeFillingTriangles.vs");
	shader_computeNormals.attachGeometryShader("shaders/VolumeFillingTriangles.gs");
	shader_computeNormals.attachFragmentShader("shaders/ComputeNormals.fs");
	shader_computeNormals.link();
}
//---------------------------------------------------------------------------------------
void LightingOven::setShaderUniforms (
		const glm::uvec3 & densityTextureDimensions,
		const glm::uvec3 & normalAmboTextureDimensions
 ) {
	vec3 dim;
	dim.x = densityTextureDimensions.x;
	dim.y = densityTextureDimensions.y;
	dim.z = densityTextureDimensions.z;
	shader_computeNormals.setUniform("inv_densityWidth", 1.0f / dim.x);
	shader_computeNormals.setUniform("inv_densityHeight", 1.0f / dim.y);
	shader_computeNormals.setUniform("inv_densityDepth", 1.0f / dim.z);

	shader_computeNormals.setUniform("normalAmboDim", vec3(normalAmboTextureDimensions));

	GLint textureUnitOffset = 0;
	shader_computeNormals.setUniform("densityTexture", textureUnitOffset);
	shader_computeNormals.setUniform("textureDepth", dim.z);
}

//---------------------------------------------------------------------------------------
// Renders normals into normalAmboTexture member of TerrainBlock
void LightingOven::bakeNormals(TerrainBlock & block) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//-- Disable depth testing and writing to depth buffer:
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	// Bind a junk VAO so driver doesn't complain.
	glBindVertexArray(junkVao);

	const Texture3D & densityTexture = *(block.densityTexture);
	const Texture3D & normalAmboTexture = *(block.normalAmboTexture);

	glActiveTexture(GL_TEXTURE0);
	densityTexture.bind();

	GLint prevViewportData[4];
	glGetIntegerv(GL_VIEWPORT, prevViewportData);

	// Set viewport size to match normalAmboTexture--the texture to be written to.
	glViewport(0, 0, normalAmboTexture.width(), normalAmboTexture.height());

	GLsizei numInstances = normalAmboTexture.depth();

	shader_computeNormals.enable();
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, numInstances);
	shader_computeNormals.disable();

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
void LightingOven::bakeAmbientOcclusion(TerrainBlock & block) {

	// TODO Dustin - Implement this...

}

//---------------------------------------------------------------------------------------
void LightingOven::setTextureRenderTarget(
		const Synergy::Texture3D &texture
) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.name(), 0);

	CHECK_FRAMEBUFFER_COMPLETENESS;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
