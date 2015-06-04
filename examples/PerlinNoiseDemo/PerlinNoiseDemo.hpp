/**
* Renders Perlin Noise to texture 3d texture
*/

#pragma once

#include <Synergy/Synergy.hpp>

#include "GlfwOpenGlWindow.hpp"
#include "PerlinNoiseGenerator.hpp"

const int kScreenWidth = 800;
const int kScreenHeight = 600;

const GLuint kNoiseWidth = 16;
const GLuint kNoiseHeight = 16;
const GLuint kNoiseDepth = 16;

class PerlinNoiseDemo : public GlfwOpenGlWindow {
public:
	PerlinNoiseDemo();
	~PerlinNoiseDemo();

	static std::shared_ptr<GlfwOpenGlWindow> getInstance();

private:
	// Needed for full viewport rendering using a single
	// triangle and no VBOs.
	GLuint junkVao;

	Synergy::ShaderProgram shader_screenQuad;

	PerlinNoiseGenerator * perlinGenerator;

	Synergy::Texture3D noiseTexture3d;

	void init();
	void logic();
	void draw();
	void cleanup();
	void setupGl();

	void setupShaderProgram();

	void inspectTextureData(const Synergy::Texture3D & texture);

	void renderTextureToScreen(const Synergy::Texture3D & texture);
};