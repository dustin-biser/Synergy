/// RenderTarget

#include <Synergy/Synergy.hpp>


class RenderTarget {
public:
	RenderTarget(Synergy::uint32 width, Synergy::uint32 height);

	~RenderTarget();

	void bind() const;

	void unbind() const;

	const Synergy::Texture2D & getColorAttachment() const;

	void clear(GLbitfield mask);

	GLuint getWidth() const;

	GLuint getHeight() const;

private:
	GLuint framebuffer;
	GLuint depthRenderBuffer;
	GLuint width;
	GLuint height;

	Synergy::Texture2D colorTexture;

	void createTextureStorage();

	void createDepthBufferStorage();

	void initFramebuffer();
};
