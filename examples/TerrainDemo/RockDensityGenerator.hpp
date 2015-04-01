/**
* RockDensityGenerator
*/

#pragma once

#include <Synergy/Synergy.hpp>

// Forward Declaration
class TerrainBlock;



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

	Synergy::ShaderProgram shader_computeRockDensity;

	// Needed to make certain drivers happy when doing screen quad
	// rendering using no vbo data.
	GLuint junkVao;


	void setupShaderProgram();

	void computeRockDensity();

	void generateBasicRock(TerrainBlock & block);

};