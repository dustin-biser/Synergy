#include "TerrainRenderer.hpp"
#include "TerrainBlock.hpp"
#include "RenderTarget.hpp"

using namespace glm;

//----------------------------------------------------------------------------------------
TerrainRenderer::TerrainRenderer (
		const uvec3 & densityGridDimensions
) {
	glGenVertexArrays(1, &vao_terrainSurface);

	uvec3 dim = densityGridDimensions - uvec3(1);
	numVoxelsPerBlock = dim.x * dim.y * dim.z;

	setupShaderPrograms(densityGridDimensions);
	setupBlockEdgesVertexBuffer();
	setupBlockEdgesVao();
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::render(
		const Synergy::Camera & camera,
		const TerrainBlock & block,
		const RenderTarget * renderTarget
) {
	if(renderTarget) {
		renderTarget->bind();
	}

	updateShaderUniforms(camera, block);
	setVertexAttributeMappings(block);
	renderIsoSurface(block);

	if (visualizeBlockEdges) {
		renderBlockEdges(block);
	}

	if (visualizeVoxelEdges) {
		renderVoxelEdges();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::setupShaderPrograms (
		const uvec3 & densityGridDimensions
) {

	//-- shader_terrainSurface:
	{
		shader_terrainSurface.generateProgramObject();
		shader_terrainSurface.attachVertexShader ("shaders/RenderTerrainSurface.vs");
		shader_terrainSurface.attachFragmentShader ("shaders/RenderTerrainSurface.fs");
		shader_terrainSurface.link();
	}


	//-- shader_blockEdges:
	{
		shader_blockEdges.generateProgramObject();
		shader_blockEdges.attachVertexShader("shaders/BlockEdges.vs");
		shader_blockEdges.attachFragmentShader("shaders/BlockEdges.fs");
		shader_blockEdges.link();

		shader_blockEdges.setUniform("lineColor", vec3(0.7f, 0.7f, 0.7f));
	}

	//-- shader_voxelEdges:
	{
		shader_voxelEdges.generateProgramObject();
		shader_voxelEdges.attachVertexShader("shaders/VoxelEdges.vs");
		shader_voxelEdges.attachFragmentShader("shaders/VoxelEdges.fs");
		shader_voxelEdges.link();

		float numVoxelCols = densityGridDimensions.x - 1.0f;
		float numVoxelRows = densityGridDimensions.y - 1.0f;
		float numVoxelLayers = densityGridDimensions.z - 1.0f;

		shader_voxelEdges.setUniform("numVoxelCols", numVoxelCols);
		shader_voxelEdges.setUniform("numVoxelRows", numVoxelRows);
		shader_voxelEdges.setUniform("numVoxelLayers", numVoxelLayers);
		shader_voxelEdges.setUniform("lineColor", vec3(0.5f, 0.5f, 0.5f));
	}
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::updateShaderUniforms (
		const Synergy::Camera & camera,
		const TerrainBlock & block
) {
	mat4 projMatrix = camera.getProjectionMatrix();
	mat4 viewMatrix = camera.getViewMatrix();
	mat4 vpMatrix = projMatrix * viewMatrix;

	mat4 modelMatrix;
	modelMatrix[3] = vec4(block.getMinVertexPos(), 1.0);

	shader_terrainSurface.setUniform("MVP_Matrix", vpMatrix * modelMatrix);
	shader_terrainSurface.setUniform("NormalMatrix", glm::transpose(glm::inverse(viewMatrix)));

	shader_blockEdges.setUniform("ViewProjMatrix", vpMatrix);

	shader_voxelEdges.setUniform("ViewProjMatrix", vpMatrix);
	shader_voxelEdges.setUniform("wsBlockMinVertexPos", block.getMinVertexPos());
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::renderIsoSurface(
		const TerrainBlock & block
) {
	glBindVertexArray(vao_terrainSurface);

	GLuint transformFeedbackObj = block.getTransformFeedbackObj();

	// Contains transform feedback primitive count needed for draw call.
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedbackObj);

	shader_terrainSurface.enable();
		glDrawTransformFeedback(GL_TRIANGLES, transformFeedbackObj);
	shader_terrainSurface.disable();

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::renderBlockEdges(
		const TerrainBlock &block
) {
	shader_blockEdges.setUniform("wsBlockMinVertPos", block.getMinVertexPos());

	glBindVertexArray(vao_blockEdges);

	shader_blockEdges.enable();
		glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, nullptr);
	shader_blockEdges.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::renderVoxelEdges() {
	glBindVertexArray(vao_blockEdges);

	shader_voxelEdges.enable();
	  glDrawElementsInstanced(GL_LINES, 24, GL_UNSIGNED_SHORT, nullptr, numVoxelsPerBlock);
	shader_voxelEdges.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;

}

//----------------------------------------------------------------------------------------
void TerrainRenderer::setupBlockEdgesVertexBuffer() {

	//   World-Space Axes
	//      y
	//      |
	//      |   / -z
	//      |  /
	//      | /
	//      O -------- x

	//  Voxel vertex offsets in world-space
	float32 voxelVertices[] = {
			0,0,0,  // 0 left bottom front
			1,0,0,  // 1 right bottom front
			1,0,-1, // 2 right bottom back
			0,0,-1, // 3 left bottom back

			0,1,0,  // 4 left top front
			1,1,0,  // 5 right top front
			1,1,-1, // 6 right top back
			0,1,-1, // 7 left top back
	};
	// Upload Vertex Position Data:
	glGenBuffers(1, &vbo_blockEdges);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_blockEdges);
	glBufferData(GL_ARRAY_BUFFER, sizeof(voxelVertices), voxelVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;


	// Voxel corner indices for constructing GL_LINES.
	// Every two indices is a new line.
	GLushort indices [] = {
			0,1, 1,2, 2,3, 3,0,
			4,5, 5,6, 6,7, 7,4,
			0,4, 1,5, 2,6, 3,7

	};

	// Upload Index Data:
	glGenBuffers(1, &ibo_blockEdges);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_blockEdges);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::setupBlockEdgesVao() {
	glGenVertexArrays(1, &vao_blockEdges);
	glBindVertexArray(vao_blockEdges);

	// Set index buffer binding for VAO.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_blockEdges);

	GLuint position_attrib_index = 0;

	// Map position buffer data into vertex attribute index.
	glBindBuffer(GL_ARRAY_BUFFER, vbo_blockEdges);
	glEnableVertexAttribArray(position_attrib_index);
	glVertexAttribPointer(position_attrib_index, 3, GL_FLOAT, GL_FALSE, 0,
			reinterpret_cast<void *>(0));


	//-- Restore defaults:
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::setVertexAttributeMappings(
		const TerrainBlock & block
) {
	glBindVertexArray(vao_terrainSurface);
	glEnableVertexAttribArray(position_attrib_index);
	glEnableVertexAttribArray(normal_attrib_index);

	//-- Map vertex buffers to vertex attribute array indices:
	{
		glBindBuffer(GL_ARRAY_BUFFER, block.getPositionVertexBuffer());
		glVertexAttribPointer(position_attrib_index, 3, GL_FLOAT,
				GL_FALSE, 0, reinterpret_cast<void *>(0));

		glBindBuffer(GL_ARRAY_BUFFER, block.getNormalVertexBuffer());
		glVertexAttribPointer(normal_attrib_index, 3, GL_FLOAT,
				GL_FALSE, 0, reinterpret_cast<void *>(0));
	}


	//-- Restore Defaults:
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::enableVisualizeNormals() {
	visualizeNormals = true;
	shader_terrainSurface.setUniform("visualizeNormals", visualizeNormals);
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::disableVisualizeNormals() {
	visualizeNormals = false;
	shader_terrainSurface.setUniform("visualizeNormals", visualizeNormals);
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::enableVisualizeBlocks() {
	visualizeBlockEdges = true;
}

//----------------------------------------------------------------------------------------
void TerrainRenderer::disableVisualizeBlocks() {
	visualizeBlockEdges = false;
}
