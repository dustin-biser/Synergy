#include "PerlinNoiseDemo.hpp"
using std::shared_ptr;

using namespace Synergy;
using namespace glm;


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
	setupShaderProgram();

	perlinGenerator =
			new PerlinNoiseGenerator(uvec3(kNoiseWidth, kNoiseHeight, kNoiseDepth));
	perlinGenerator->getNoiseTexture(noiseTexture3d);
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
void PerlinNoiseDemo::setupShaderProgram() {
	shader_screenQuad.generateProgramObject();
	shader_screenQuad.attachVertexShader("shaders/ScreenQuad.vs");
	shader_screenQuad.attachFragmentShader("shaders/ScreenQuad.fs");
	shader_screenQuad.link();

	shader_screenQuad.setUniform("texture3D", 0);
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::renderTextureToScreen(const Texture3D & texture) {
	glActiveTexture(GL_TEXTURE0);
	texture.bind();

	shader_screenQuad.setUniform("layer", 0);

	glBindVertexArray(junkVao);

	shader_screenQuad.enable();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	shader_screenQuad.disable();


	glBindVertexArray(0);
	texture.unbind();
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::logic() {
//	inspectTextureData(noise_texture3d);
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::draw() {
	renderTextureToScreen(noiseTexture3d);
}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::cleanup() {

}

//---------------------------------------------------------------------------------------
void PerlinNoiseDemo::inspectTextureData(const Texture3D & texture) {
	int width = texture.width();
	int height = texture.height();
	int depth = texture.depth();
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

	texture.bind();
	glGetTexImage(GL_TEXTURE_3D, 0, texture.format(), texture.dataType(), data);

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
	texture.unbind();
	CHECK_GL_ERRORS;
}
