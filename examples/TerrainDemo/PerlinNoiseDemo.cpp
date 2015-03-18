#include "PerlinNoiseDemo.hpp"
using std::shared_ptr;
using namespace Synergy;


// TODO Dustin - remove after testing:
#include <iostream>
using namespace std;

//---------------------------------------------------------------------------------------
int main() {
	shared_ptr<GlfwOpenGlWindow> demo =  PerlinNoiseDemo::getInstance();
	demo->create(kScreenWidth, kScreenHeight, "Perlin Noise Demo", 1/60.0f);

	return 0;
}


//---------------------------------------------------------------------------------------
shared_ptr<GlfwOpenGlWindow> PerlinNoiseDemo::getInstance() {
	if(p_instance == nullptr) {
		p_instance = shared_ptr<GlfwOpenGlWindow>(new PerlinNoiseDemo());
	}
	return p_instance;
}

//---------------------------------------------------------------------------------------
PerlinNoiseDemo::PerlinNoiseDemo() {

}

//---------------------------------------------------------------------------------------
PerlinNoiseDemo::~PerlinNoiseDemo() {

}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::init() {
	glGenVertexArrays(1, &junkVao);

	initShaders();
	setShaderUniforms();

	initNoiseHashTableTextureBuffer();

	generateNoiseTexture3d();
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::setupGl() {
	// Render only the front face of geometry.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	// Setup depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glEnable(GL_DEPTH_CLAMP);

	glClearDepth(1.0f);
	glClearColor(0.3, 0.5, 0.7, 1.0);
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::initShaders() {
	shader_screenQuad.generateProgramObject();
	shader_screenQuad.attachVertexShader("shaders/ScreenQuad.vs");
	shader_screenQuad.attachFragmentShader("shaders/ScreenQuad.fs");
	shader_screenQuad.link();

	shader_perlinNoise.generateProgramObject();
	shader_perlinNoise.attachVertexShader("shaders/PerlinNoise.vs");
	shader_perlinNoise.attachGeometryShader("shaders/PerlinNoise.gs");
	shader_perlinNoise.attachFragmentShader("shaders/PerlinNoise.fs");
	shader_perlinNoise.link();
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::setShaderUniforms() {
	shader_screenQuad.setUniform("texture3D", 0);

	shader_perlinNoise.setUniform("hashTable", 0);
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::renderTextureToScreen(const Texture & texture) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texture.type, texture.id);

	shader_screenQuad.setUniform("layer", 0);

	glBindVertexArray(junkVao);

	shader_screenQuad.enable();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	shader_screenQuad.disable();


	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::initNoiseHashTableTextureBuffer() {
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
void PerlinNoiseDemo::generateNoiseTexture3d() {
	createTextureStorage(noise_texture3d);

	initFramebufferWithRenderTarget(noise_texture3d);

	fillNoiseTexture3d(noise_texture3d);
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::createTextureStorage(Texture & texture) {
	glGenTextures(1, &texture.id);
	glBindTexture(texture.type, texture.id);
	glTexImage3D(
			texture.type,
			0,
			texture.internalFormat,
			texture.width,
			texture.height,
			texture.depth,
			0,
			texture.format,
			texture.dataType,
			nullptr
	);

	glTexParameteri(texture.type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texture.type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(texture.type, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(texture.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(texture.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(texture.type, 0);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::initFramebufferWithRenderTarget(const Texture & texture) {
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.id, 0);


	CHECK_FRAMEBUFFER_COMPLETENESS;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//---------------------------------------------------------------------------------------
/**
* Renders viewport filling triangles without any vbo data.
* Each instance fills one layer of attached 3d texture render target.
*/
void PerlinNoiseDemo::fillNoiseTexture3d(const Texture & texture) {
	glViewport(0, 0, texture.width, texture.height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_BUFFER, texture_noiseHashTable);

	// Bind junkVao to make gl driver happy.
	glBindVertexArray(junkVao);

	shader_perlinNoise.enable();
	GLsizei numInstances = texture.depth;
	// Each instance is a viewport filling triangle without using vbo data.
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, numInstances);
	shader_perlinNoise.disable();

	//-- Reset defaults:
	glViewport(0, 0, defaultFramebufferWidth(), defaultFramebufferHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::logic() {
	inspectTextureData(noise_texture3d);
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::draw() {
	renderTextureToScreen(noise_texture3d);
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::cleanup() {

}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::inspectTextureData(const Texture & texture) {
	int width = texture.width;
	int height = texture.height;
	int depth = texture.depth;
	float * data = new float[width * height * depth];
	for(int i(0); i < width; ++i) {
		for(int j(0); j < height; ++j) {
			for(int k(0); k < depth; ++k) {
				// Zero out data
				data[k*(width*height) + j*width + i] = 0.0f;
			}
		}
	}

	glFlush();
	glFinish();
	glBindTexture(texture.type, texture.id);
	glGetTexImage(texture.type, 0, texture.format, texture.dataType, data);

	float min = FLT_MAX;
	float max = FLT_MIN;
	float value;
	for(int i(0); i < width; ++i) {
		for(int j(0); j < height; ++j) {
			for(int k(0); k < depth; ++k) {
				value = data[k*(width*height) + j*width + i];
				min = (value < min) ? value : min;
				max = (value > max) ? value : max;
			}
		}
	}

	cout << "min: " << min << endl;
	cout << "max: " << max << endl;

	delete [] data;
	glBindTexture(texture.type, 0);
	CHECK_GL_ERRORS;
}
