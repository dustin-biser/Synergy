/// RenderTarget

#include <Synergy/Synergy.hpp>


class RenderTarget {
public:
	RenderTarget(Synergy::uint32 width, Synergy::uint32 height);

	~RenderTarget();

	void bind() const;

	void unbind() const;

	Synergy::Texture2D getColorAttachment();

private:
	GLuint framebuffer;
	GLuint depthRenderBuffer;

	Synergy::Texture2D colorTexture;

	void createTextureStorage(
			Synergy::uint32 width,
			Synergy::uint32 height
	);

	void createDepthBufferStorage(
			Synergy::uint32 width,
			Synergy::uint32 height
	);

	void initFramebuffer();

	void clearColorDepthAttachments(
			Synergy::uint32 width,
			Synergy::uint32 height
	);
};
