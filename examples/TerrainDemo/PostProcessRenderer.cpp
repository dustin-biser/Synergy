#include "PostProcessRenderer.hpp"
#include "RenderTarget.hpp"


//---------------------------------------------------------------------------------------
PostProcessRenderer::PostProcessRenderer() {
	setupShaderProgram();
	glGenVertexArrays(1, &junkVao);
}

//---------------------------------------------------------------------------------------
PostProcessRenderer::~PostProcessRenderer() {
	glDeleteVertexArrays(1, &junkVao);
}

//---------------------------------------------------------------------------------------
void PostProcessRenderer::render (
		const RenderTarget & renderTarget
) {
	GLint prevViewportData[4];
	glGetIntegerv(GL_VIEWPORT, prevViewportData);

	glViewport(0, 0, renderTarget.getWidth(), renderTarget.getHeight());

	glActiveTexture(GL_TEXTURE0);
	renderTarget.getColorAttachment().bind();

	glBindVertexArray(junkVao);

	shader_renderTexture.enable();
		glDrawArrays(GL_TRIANGLES, 0, 3);
	shader_renderTexture.disable();


	renderTarget.getColorAttachment().unbind();
	glViewport(prevViewportData[0], prevViewportData[1], prevViewportData[2],
			prevViewportData[3]);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void PostProcessRenderer::setupShaderProgram() {
	//-- shader_renderTexture:
	{
		shader_renderTexture.generateProgramObject();
		shader_renderTexture.attachVertexShader("shaders/ScreenQuad.vs");
		shader_renderTexture.attachFragmentShader("shaders/RenderTexture.fs");
		shader_renderTexture.link();

		shader_renderTexture.setUniform("texture2D", 0);
	}

	//-- shader_gammaCorrection:
	{
		shader_gammaCorrection.generateProgramObject();
		shader_gammaCorrection.attachVertexShader("shaders/ScreenQuad.vs");
		shader_gammaCorrection.attachFragmentShader("shaders/GammaCorrection.fs");
		shader_gammaCorrection.link();

		shader_gammaCorrection.setUniform("texture2D", 0);
	}

}

//---------------------------------------------------------------------------------------
void PostProcessRenderer::applyGammaCorrection(
		const RenderTarget & renderTarget,
		float gamma
) {
	GLint prevViewportData[4];
	glGetIntegerv(GL_VIEWPORT, prevViewportData);

	glViewport(0, 0, renderTarget.getWidth(), renderTarget.getHeight());

	glActiveTexture(GL_TEXTURE0);
	renderTarget.getColorAttachment().bind();

	shader_gammaCorrection.setUniform("inv_gamma", 1.0f/gamma);

	glBindVertexArray(junkVao);

	shader_gammaCorrection.enable();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	shader_gammaCorrection.disable();


	renderTarget.getColorAttachment().unbind();
	glViewport(prevViewportData[0], prevViewportData[1], prevViewportData[2],
			prevViewportData[3]);
	CHECK_GL_ERRORS;
}
