/// LightingOven

#include <Synergy/Synergy.hpp>
#import "TerrainBlock.hpp"

class LightingOven {
public:
	LightingOven(const glm::uvec3 & densityGridDimensions);

	~LightingOven();

	void bakeNormals(TerrainBlock & block);
	void bakeAmbientOcclusion(TerrainBlock & block);

private:
	// Junk vao for processing normalAmbo_texture3d within
	// computeNormalAmboTexture() using no VBO data.
	GLuint junkVao;

	GLuint framebuffer;

	Synergy::ShaderProgram shader_computeNormals;

	void setupShaderProgram();
	void setShaderUniforms(const glm::uvec3 & densityGridDimensions);

	void computeNormals(TerrainBlock & block);

	void computeAmbientOcclusion(TerrainBlock & block);

	void setFramebufferColorAttachment(const Synergy::Texture3D & texture);



	// TODO Dustin - Remove after testing:
	void inspectTextureData(const TerrainBlock & block);

};