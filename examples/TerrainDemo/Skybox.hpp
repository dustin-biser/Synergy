/// Skybox

#pragma once

#include <Synergy/Synergy.hpp>

class Skybox {
public:
	Skybox();

	~Skybox();

	void loadCubeMap(
		const char * front,
		const char * back,
		const char * left,
		const char * right,
		const char * up,
		const char * down
	);

	void render(const Synergy::Camera & camera);

private:
	GLuint vao_skybox;
	GLuint vbo_positions;
	GLuint cubeMapTexture;

	Synergy::ShaderProgram shader;

	void uploadSkyboxVertexData();
	void setVertexAttributeMappings();
	void setupShaderProgram();

	void updateShaderUniforms(const Synergy::Camera & camera);

	void loadCubeMapTexture(
			GLuint texture,
			GLenum sideTarget,
			const char *  fileName
	);

};