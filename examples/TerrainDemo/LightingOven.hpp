/// LightingOven

#include <Synergy/Synergy.hpp>
#import "TerrainBlock.hpp"

class LightingOven {
public:
	LightingOven(
			const glm::uvec3 &densityTextureDimensions,
			const glm::uvec3 &normalAmboTextureDimensions
	);

	~LightingOven();

	void bakeNormals(TerrainBlock & block);

	void bakeAmbientOcclusion(TerrainBlock & block);

	void setTextureRenderTarget(
			const Synergy::Texture3D & texture
	);

private:
	// Junk vao for processing normalAmbo_texture3d within
	// computeNormalAmboTexture() using no VBO data.
	GLuint junkVao;

	GLuint framebuffer;

	Synergy::ShaderProgram shader_computeNormals;

	void setupShaderProgram();

	void setShaderUniforms(
			const glm::uvec3 & densityTextureDimensions,
			const glm::uvec3 & normalAmboTextureDimensions
	);
};
