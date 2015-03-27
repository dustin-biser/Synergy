/**
* Volume Filling Triangles
*
* Create instances of viewport filling triangles with no VBO data so that
* each texel of a 3D texture can be processed within a fragment shader.
*
* Goal is to copy a 3D source texture into a 3D destination texture
* with the destination texture being a framebuffer color attachment.
*/

#pragma once

#include <Synergy/Synergy.hpp>
using namespace Synergy;

#include "GlfwOpenGlWindow.hpp"

const int kTextureWidth = 2;
const int kTextureHeight = 2;
const int kTextureDepth = 2;


class VolumeFillingTriangles : public GlfwOpenGlWindow {
public:
	static std::shared_ptr<GlfwOpenGlWindow> getInstance();

private:

	Synergy::Texture3D sourceTexture;  // Source Texture
	Synergy::Texture3D destTexture;    // Destination Texture

	Synergy::ShaderProgram shader_copyTexture;

	GLuint framebuffer;

	GLuint junkVao;

	void init();
	void logic();
	void draw();

	void allocateTextureStorage();
	void uploadSourceTextureData();
	void initFramebufferWithColorAttachment (
			const Texture3D & texture
	);
	void setupShaderProgram();

	void renderSourceTextureIntoDestTexture();

	void inspectDestTexture();

};