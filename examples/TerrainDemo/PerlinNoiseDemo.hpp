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

struct Texture {
	GLenum type;
	GLuint id;
	GLuint width;
	GLuint height;
	GLuint depth;
	GLenum internalFormat;
	GLenum format;
	GLenum dataType;
};

Texture noise_texture3d = {
		GL_TEXTURE_3D,
		0,
		16,
		16,
		16,
		GL_R16F,
		GL_RED,
		GL_FLOAT
};

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
	void createTextureStorage(Texture & texture);
	void initFramebufferWithRenderTarget(Texture const & texture);

	void setShaderUniforms();
	void fillNoiseTexture3d(const Texture & texture);

	void generateNoiseTexture3d();

	void inspectTextureData(const Texture & texture);

	void renderTextureToScreen(const Texture & texture);

};