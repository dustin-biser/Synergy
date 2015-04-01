/// PostProcessRenderer

#pragma once

#include <Synergy/Synergy.hpp>

// Forward Declaration
class RenderTarget;


class PostProcessRenderer {
public:
	PostProcessRenderer();

	~PostProcessRenderer();

	void render(const RenderTarget & renderTarget);

	void applyGammaCorrection(
			const RenderTarget & renderTarget,
			float gamma
	);

private:
	GLuint junkVao;

	Synergy::ShaderProgram shader_renderTexture;
	Synergy::ShaderProgram shader_gammaCorrection;

	void setupShaderProgram();

};