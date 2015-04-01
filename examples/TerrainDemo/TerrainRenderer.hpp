#pragma once

#include <Synergy/Synergy.hpp>

// Forward Declaration
class TerrainBlock;
class RenderTarget;


class TerrainRenderer {
public:
	TerrainRenderer(const glm::uvec3 & densityGridDimensions);

	void render(
			const Synergy::Camera & camera,
			const TerrainBlock & block,
			const RenderTarget * renderTarget = nullptr
	);

	void enableVisualizeNormals();
	void disableVisualizeNormals();

	void enableVisualizeBlocks();
	void disableVisualizeBlocks();

private:
	bool visualizeNormals;
	bool visualizeBlocks;

	//-- Vertex Array Attribute Indices:
	const GLuint position_attrib_index = 0;
	const GLuint normal_attrib_index = 1;

	Synergy::ShaderProgram shader_terrainSurface;
	Synergy::ShaderProgram shader_blockEdges;

	//-- For Block Edge Rendering:
	GLuint vao_blockEdges;
	GLuint vbo_blockEdges; // vertex buffer object
	GLuint ibo_blockEdges; // index buffer object

	GLuint vao_terrainSurface;

	Synergy::uint32 numVoxelsPerBlock;

	void setupShaderPrograms(const glm::uvec3 & densityGridDimensions);
	void setupBlockEdgesVertexBuffer();
	void setupBlockEdgesVao();
	void setVertexAttributeMappings(const TerrainBlock & block);

	void updateShaderUniforms(const Synergy::Camera & camera);

	void renderIsoSurface(
			const TerrainBlock & block
	);

	void renderBlockEdges(
			const TerrainBlock &block
	);

};