#include "PerlinNoiseGenerator.hpp"

using namespace Synergy;
using namespace glm;


//---------------------------------------------------------------------------------------
PerlinNoiseGenerator::PerlinNoiseGenerator(
		const uvec3 & noiseTextureDimensions
) {
	glGenVertexArrays(1, &junkVao);

	setupShaderProgram();

	initNoiseHashTableTextureBuffer();

	createTextureStorage(noiseTextureDimensions);
	initFramebufferWithRenderTarget();
	renderNoiseToTexture();

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
PerlinNoiseGenerator::~PerlinNoiseGenerator() {

}

//---------------------------------------------------------------------------------------
void PerlinNoiseGenerator::setupShaderProgram() {
	shader_perlinNoise.generateProgramObject();
	shader_perlinNoise.attachVertexShader("../Utils/shaders/PerlinNoise.vs");
	shader_perlinNoise.attachGeometryShader("../Utils/shaders/PerlinNoise.gs");
	shader_perlinNoise.attachFragmentShader("../Utils/shaders/PerlinNoise.fs");
	shader_perlinNoise.link();

	shader_perlinNoise.setUniform("hashTable", 0);
}

//---------------------------------------------------------------------------------------
void PerlinNoiseGenerator::initNoiseHashTableTextureBuffer() {
	glGenBuffers(1, &tbo_noiseHashTable);
	glBindBuffer(GL_TEXTURE_BUFFER, tbo_noiseHashTable);

	// Hash lookup table as defined by Ken Perlin.  This is a randomly
	// arranged array of all numbers from 0-255 inclusive.
	uint8 hashTable[] = {
			151,160,137,91,90,15,
			131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
			190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
			88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
			77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
			102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
			135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
			5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
			223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
			129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
			251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
			49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
			138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	//-- Write two repeating copies of hashTable into buffer storage.
	{
		uint8 repeatedHashTable[512];
		for (int i = 0; i < 512; ++i) {
			// Make two repeating copies of hashTable.
			repeatedHashTable[i] = hashTable[i % 256];
		}

		// Data will only be used by GL, and never read back or modified.
		glBufferData(GL_TEXTURE_BUFFER, sizeof(repeatedHashTable), repeatedHashTable,
				GL_STATIC_DRAW);
	}


	//-- Attach buffer object's data store to texture object
	{
		glGenTextures(1, &texture_noiseHashTable);
		glBindTexture(GL_TEXTURE_BUFFER, texture_noiseHashTable);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R8UI, tbo_noiseHashTable);
	}


	//-Reset Defaults
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	CHECK_GL_ERRORS;
}


//---------------------------------------------------------------------------------------
void PerlinNoiseGenerator::createTextureStorage (
	const uvec3 & noiseTextureDimensions
) {
	TextureSpec textureSpec;
	textureSpec.width = noiseTextureDimensions.x;
	textureSpec.height = noiseTextureDimensions.y;
	textureSpec.depth = noiseTextureDimensions.z;
	textureSpec.internalFormat = GL_R32F;
	textureSpec.format = GL_RED;
	textureSpec.dataType = GL_FLOAT,

	noiseTexture3d.allocateStorage(textureSpec);

	noiseTexture3d.bind();
	glTexParameteri(noiseTexture3d.type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(noiseTexture3d.type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(noiseTexture3d.type, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(noiseTexture3d.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(noiseTexture3d.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	noiseTexture3d.unbind();
}

//---------------------------------------------------------------------------------------
void PerlinNoiseGenerator::initFramebufferWithRenderTarget() {
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, noiseTexture3d.name(), 0);


	CHECK_FRAMEBUFFER_COMPLETENESS;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//---------------------------------------------------------------------------------------
/**
* Renders viewport filling triangles without any vbo data.
* Each instance fills one layer of attached 3d texture render target.
*/
void PerlinNoiseGenerator::renderNoiseToTexture() {
	//-- Get previous glViewport settings
	GLint prevViewportData[4];
	glGetIntegerv(GL_VIEWPORT, prevViewportData);

	glViewport(0, 0, noiseTexture3d.width(), noiseTexture3d.height());
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, texture_noiseHashTable);

	// Bind junkVao to make gl driver happy.
	glBindVertexArray(junkVao);

	shader_perlinNoise.enable();
	GLsizei numInstances = noiseTexture3d.depth();
	// Each instance is a viewport filling triangle without using vbo data.
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, numInstances);
	shader_perlinNoise.disable();

	//-- Reset defaults:
	glViewport(prevViewportData[0], prevViewportData[1], prevViewportData[2],
			prevViewportData[3]);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void PerlinNoiseGenerator::getNoiseTexture(
		Synergy::Texture3D & texture
) const {
	texture = noiseTexture3d;
}
