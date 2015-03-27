/**
* Renders Perlin Noise to texture 3d texture
*/

#pragma once

#include "GlfwOpenGlWindow.hpp"
#include <Synergy/Synergy.hpp>

const int kScreenWidth = 800;
const int kScreenHeight = 600;

const GLint kAttribIndex_positions = 0;
const GLint kAttribIndex_texCoords = 0;

Synergy::TextureSpec noise_texture3dspec = {
		16,
		16,
		16,
		GL_R16F,
		GL_RED,
		GL_FLOAT
};
Synergy::Texture3D noise_texture3d;

class PerlinNoiseDemo : public GlfwOpenGlWindow {
public:
	PerlinNoiseDemo();
	~PerlinNoiseDemo();

	static std::shared_ptr<GlfwOpenGlWindow> getInstance();

private:
	GLuint framebuffer;

	// Needed for full viewport rendering using a single
	// triangle and no VBOs.
	GLuint junkVao;

	// Buffer backed storage for texture noise hash table.
	// Needs both texture data and a texture buffer object
	// for storing texture data in buffer.
	GLuint tbo_noiseHashTable;     // Texture buffer object
	GLuint texture_noiseHashTable; // Texture


	Synergy::ShaderProgram shader_screenQuad;
	Synergy::ShaderProgram shader_perlinNoise;


	void init();
	void logic();
	void draw();
	void cleanup();
	void setupGl();

	void initShaders();
	void initNoiseHashTableTextureBuffer();
	void createTextureStorage(Synergy::Texture3D & texture);
	void initFramebufferWithRenderTarget(Synergy::Texture3D const & texture);

	void setShaderUniforms();
	void fillNoiseTexture3d(const Synergy::Texture3D & texture);

	void generateNoiseTexture3d();

	void inspectTextureData(const Synergy::Texture3D & texture);

	void renderTextureToScreen(const Synergy::Texture3D & texture);

};