#include "VolumeFillingTriangles.hpp"

using std::shared_ptr;

//---------------------------------------------------------------------------------------
int main() {
	shared_ptr<GlfwOpenGlWindow> demo =  VolumeFillingTriangles::getInstance();
	demo->create(800, 600, "Volume Filling Triangles Demo", 1/60.0f);

	return 0;
}


//---------------------------------------------------------------------------------------
shared_ptr<GlfwOpenGlWindow> VolumeFillingTriangles::getInstance() {
	if(p_instance == nullptr) {
		p_instance = shared_ptr<GlfwOpenGlWindow>(new VolumeFillingTriangles());
	}
	return p_instance;
}

//---------------------------------------------------------------------------------------
void VolumeFillingTriangles::init() {
	glGenVertexArrays(1, &junkVao);

	allocateTextureStorage();
	uploadSourceTextureData();

	initFramebufferWithColorAttachment(destTexture);

	setupShaderProgram();
}

//---------------------------------------------------------------------------------------
void VolumeFillingTriangles::allocateTextureStorage() {
	TextureSpec textureSpec;
	textureSpec.width = kTextureWidth;
	textureSpec.height = kTextureHeight;
	textureSpec.depth = kTextureDepth;
	textureSpec.internalFormat = GL_R16F;
	textureSpec.format = GL_RED;
	textureSpec.dataType = GL_FLOAT;

	sourceTexture.allocateStorage(textureSpec);
	sourceTexture.bind();
		glTexParameteri(sourceTexture.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(sourceTexture.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(sourceTexture.type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(sourceTexture.type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(sourceTexture.type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	sourceTexture.unbind();

	destTexture.allocateStorage(textureSpec);
	destTexture.bind();
		glTexParameteri(destTexture.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(destTexture.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(destTexture.type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(destTexture.type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(destTexture.type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	destTexture.unbind();
}

//---------------------------------------------------------------------------------------
void VolumeFillingTriangles::initFramebufferWithColorAttachment (
		const Texture3D & texture
) {
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.name(), 0);

	CHECK_FRAMEBUFFER_COMPLETENESS;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//---------------------------------------------------------------------------------------
void VolumeFillingTriangles::setupShaderProgram() {
	shader_copyTexture.generateProgramObject();
	shader_copyTexture.attachVertexShader("shaders/VolumeFillingTriangles.vs");
	shader_copyTexture.attachGeometryShader("shaders/VolumeFillingTriangles.gs");
	shader_copyTexture.attachFragmentShader("shaders/CopyTexture.fs");
	shader_copyTexture.link();

	GLint textureUnit = 0;
	shader_copyTexture.setUniform("sourceTexture", textureUnit);
	shader_copyTexture.setUniform("textureDepth", float(kTextureDepth));
}

//---------------------------------------------------------------------------------------
void VolumeFillingTriangles::uploadSourceTextureData() {
	uint32 width = kTextureWidth;
	uint32 height = kTextureHeight;
	uint32 depth = kTextureDepth;
	float * data = new float[width * height * depth];

	for(int k(0); k < depth; ++k) {
		for (int j(0); j < height; ++j) {
			for (int i(0); i < width; ++i) {
				uint32 index = k*(width * height) + j*width + i;
				data[index] = float(index);
			}
		}
	}

	sourceTexture.bind();
	glTexSubImage3D(sourceTexture.type, 0, 0, 0, 0, sourceTexture.width(),
			sourceTexture.height(), sourceTexture.depth(), GL_RED, GL_FLOAT, data);
	sourceTexture.unbind();


	delete [] data;
}


//---------------------------------------------------------------------------------------
void VolumeFillingTriangles::logic() {

}

//---------------------------------------------------------------------------------------
void VolumeFillingTriangles::draw() {
	renderSourceTextureIntoDestTexture();

	#ifdef DEBUG
		inspectDestTexture();
	#endif
}

//---------------------------------------------------------------------------------------
/**
* Use instances of viewport filling triangles to render all layers of the
* 3D texture sourceTexture into all layers of 3D texture destTexture.
*
* Geomestry Shader selects which layer (via gl_Layer) of the framebuffer color attachement
* to write to during each instance rendering call.
*/
void VolumeFillingTriangles::renderSourceTextureIntoDestTexture() {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//-- Disable depth testing and writing to depth buffer:
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	// Bind a junk VAO so driver doesn't complain.
	glBindVertexArray(junkVao);

	glActiveTexture(GL_TEXTURE0);
	sourceTexture.bind();

	GLsizei numInstances = sourceTexture.depth();

	// Set viewport size to match destination texture, the texture we will be writing to.
	glViewport(0, 0, destTexture.width(), destTexture.height());

	shader_copyTexture.enable();
		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, numInstances);
	shader_copyTexture.disable();

	sourceTexture.unbind();
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void VolumeFillingTriangles::inspectDestTexture() {
	uint32 width = kTextureWidth;
	uint32 height = kTextureHeight;
	uint32 depth = kTextureDepth;
	float * data = new float[width * height * depth];

	destTexture.bind();
		glGetTexImage(destTexture.type, 0, destTexture.format(),
				destTexture.dataType(), data);
	destTexture.unbind();


	delete [] data;
}
