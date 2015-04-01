#pragma once

#include <Synergy/Synergy.hpp>

// Forward Declaration
class TerrainBlock;



class TerrainRenderer {
public:
	TerrainRenderer(const glm::uvec3 & densityGridDimensions);

	void render(
			const Synergy::Camera & camera,
			const TerrainBlock & block
	);

	void enableVisualizeNormals();
	void disableVisualizeNormals();

	void enableRenderVoxelEdges();
	void disableRendVoxelEdges();

private:
	bool visualizeNormals;
	bool visualizeVoxelEdges;

	//-- Vertex Array Attribute Indices:
	const GLuint position_attrib_index = 0;
	const GLuint normal_attrib_index = 1;

	Synergy::ShaderProgram shader_terrainSurface;
	Synergy::ShaderProgram shader_voxelEdges;

	//-- For Voxel Rendering:
	GLuint vao_voxelEdges;
	GLuint vbo_voxelEdges; // vertex buffer object
	GLuint ibo_voxelEdges; // index buffer object

	GLuint vao_terrainSurface;

	Synergy::uint32 numVoxelsPerBlock;

	void setupShaderPrograms(const glm::uvec3 & densityGridDimensions);
	void setupVoxelEdgesVertexBuffer();
	void setupVoxelEdgesVao();
	void setVertexAttributeMappings(const TerrainBlock & block);

	void updateShaderUniforms(const Synergy::Camera & camera);

	void renderIsoSurface(
			const TerrainBlock & block
	);

	void renderVoxelEdges (
			const TerrainBlock & block
	);

};