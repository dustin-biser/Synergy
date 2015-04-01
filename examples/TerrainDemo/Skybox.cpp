#include "Skybox.hpp"
using namespace Synergy;

#include <LoadPNG/lodepng.h>

#include <sstream>
using namespace std;

//---------------------------------------------------------------------------------------
Skybox::Skybox() {
	glGenTextures(1, &cubeMapTexture);

	uploadSkyboxVertexData();
	setVertexAttributeMappings();
	setupShaderProgram();
}

//---------------------------------------------------------------------------------------
Skybox::~Skybox() {
	glDeleteTextures(1, &cubeMapTexture);
	glDeleteVertexArrays(1, &vao_skybox);
	glDeleteBuffers(1, &vbo_positions);
}

//---------------------------------------------------------------------------------------
void Skybox::setupShaderProgram() {
	shader.generateProgramObject();
	shader.attachVertexShader("shaders/Skybox.vs");
	shader.attachFragmentShader("shaders/Skybox.fs");
	shader.link();
}


//---------------------------------------------------------------------------------------
void Skybox::uploadSkyboxVertexData() {
	float skybox_vertices[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f
	};

	glGenBuffers(1, &vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	
	GLsizei positionDataBytes = 3 * 36 * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, positionDataBytes, skybox_vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void Skybox::loadCubeMap(
		const char * front,
		const char * back,
		const char * left,
		const char * right,
		const char * up,
		const char * down
) {
	glBindTexture (GL_TEXTURE_CUBE_MAP, cubeMapTexture);

	loadCubeMapTexture(cubeMapTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, front);
	loadCubeMapTexture(cubeMapTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, back);
	loadCubeMapTexture(cubeMapTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, left);
	loadCubeMapTexture(cubeMapTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X, right);
	loadCubeMapTexture(cubeMapTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, down);
	loadCubeMapTexture(cubeMapTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, up);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glBindTexture (GL_TEXTURE_CUBE_MAP, 0);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void Skybox::loadCubeMapTexture (
		GLuint texture,
		GLenum sideTarget,
		const char *  fileName
) {
	std::vector<unsigned char> png;
	std::vector<unsigned char> imageData; //the raw pixels
	unsigned width, height;

	//load and decode
	lodepng::load_file(png, fileName);
	unsigned error = lodepng::decode(imageData, width, height, png);
	if(error) {
		stringstream stream;
		stream << "decoder error " << error << ": " << lodepng_error_text(error);
		stream << endl;
		throw Exception(stream.str());

	}

	// copy image data into 'target' side of cube map
	glTexImage2D(sideTarget, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
			reinterpret_cast<GLvoid *>(imageData.data()));

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void Skybox::setVertexAttributeMappings() {
	glGenVertexArrays(1, &vao_skybox);
	glBindVertexArray(vao_skybox);

	GLuint position_attrib_index = 0;

	glEnableVertexAttribArray(position_attrib_index);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_positions);
	glVertexAttribPointer(position_attrib_index, 3, GL_FLOAT, GL_FALSE, 0, NULL);


	//-- Restore defaults:
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void Skybox::updateShaderUniforms(const Synergy::Camera & camera) {

	// Load only the rotational part of the camera's viewMatrix:
	mat4 rotMatrix = mat4(mat3(camera.getViewMatrix()));
	shader.setUniform("viewMatrix", rotMatrix);

	shader.setUniform("projectionMatrix", camera.getProjectionMatrix());
}

//---------------------------------------------------------------------------------------
void Skybox::render(const Synergy::Camera & camera) {
	updateShaderUniforms(camera);

	// Disable writing to depth buffer
	glDepthMask(GL_FALSE);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);

	glBindVertexArray(vao_skybox);

	shader.enable();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	shader.disable();

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDepthMask(GL_TRUE);
	CHECK_GL_ERRORS;
}
