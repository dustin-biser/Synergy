#include "RenderTarget.hpp"

using namespace Synergy;

//---------------------------------------------------------------------------------------
RenderTarget::RenderTarget(
		uint32 width,
		uint32 height
)
	: width(width),
	  height(height)
{

	createTextureStorage();

	createDepthBufferStorage();

	initFramebuffer();

	clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//---------------------------------------------------------------------------------------
RenderTarget::~RenderTarget() {

}

//---------------------------------------------------------------------------------------
void RenderTarget::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void RenderTarget::unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECK_GL_ERRORS;
}


//---------------------------------------------------------------------------------------
void RenderTarget::createTextureStorage() {

	TextureSpec textureSpec;
	textureSpec.width = width;
	textureSpec.height = height;
	textureSpec.internalFormat = GL_RGBA;
	textureSpec.format = GL_RGBA;
	textureSpec.dataType = GL_UNSIGNED_INT;

	colorTexture.allocateStorage(textureSpec);

	colorTexture.bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	colorTexture.unbind();

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void RenderTarget::createDepthBufferStorage() {
	glGenRenderbuffers(1, &depthRenderBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void RenderTarget::initFramebuffer() {
	glGenFramebuffers(1, &framebuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	// Attach a specific layer of 3D color texture
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture.name(), 0);

	// Attach depthRenderBuffer as depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, depthRenderBuffer);

	CHECK_FRAMEBUFFER_COMPLETENESS;
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
const Synergy::Texture2D & RenderTarget::getColorAttachment() const {
	return colorTexture;
}

//---------------------------------------------------------------------------------------
void RenderTarget::clear(GLbitfield mask) {
	bind();
	GLint prevViewportData[4];
	glGetIntegerv(GL_VIEWPORT, prevViewportData);
	glViewport(0, 0, width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(prevViewportData[0], prevViewportData[1], prevViewportData[2],
			prevViewportData[3]);
	unbind();
}

//---------------------------------------------------------------------------------------
GLuint RenderTarget::getWidth() const {
	return width;
}

//---------------------------------------------------------------------------------------
GLuint RenderTarget::getHeight() const {
	return height;
}
