/// PerlinNoiseGenerator

#pragma once

#include <Synergy/Synergy.hpp>


class PerlinNoiseGenerator {
public:
	PerlinNoiseGenerator(const glm::uvec3 & noiseTextureDimensions);

	~PerlinNoiseGenerator();

	void getNoiseTexture(Synergy::Texture3D & texture) const;

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

	Synergy::ShaderProgram shader_perlinNoise;

	Synergy::Texture3D noiseTexture3d;


	void setupShaderProgram();

	void initNoiseHashTableTextureBuffer();

	void createTextureStorage(
			const glm::uvec3 & noiseTextureDimensions
	);

	void renderNoiseToTexture();

	void initFramebufferWithRenderTarget();

};