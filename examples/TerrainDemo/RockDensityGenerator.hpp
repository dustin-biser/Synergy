/**
* RockDensityGenerator
*/

#pragma once

#include <Synergy/Synergy.hpp>

// Forward Declaration
class TerrainBlock;
class PerlinNoiseGenerator;



class RockDensityGenerator {
public:
	RockDensityGenerator();

	~RockDensityGenerator();

	void generateRockDensity(TerrainBlock & block);

	void setTextureRenderTarget(const Synergy::Texture3D & texture);

private:
	GLuint framebuffer;

	// Target density texture to render to
	Synergy::Texture3D densityTexture;

	Synergy::Texture3D noiseTexture;

	Synergy::ShaderProgram shader_computeRockDensity;

	// Needed to make certain drivers happy when doing screen quad
	// rendering using no vbo data.
	GLuint junkVao;


	PerlinNoiseGenerator * perlinNoiseGenerator;

	void setupShaderProgram();

	void computeRockDensity();



	// TODO Dustin - Remove after testing:
	void generateBasicRock(TerrainBlock & block);
};