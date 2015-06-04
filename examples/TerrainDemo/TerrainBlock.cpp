#include "TerrainBlock.hpp"

using namespace glm;


//---------------------------------------------------------------------------------------
TerrainBlock::TerrainBlock(
		const Synergy::Texture3D & densityTexture,
		const Synergy::Texture3D & normalAmboTexture,
		uint32 bytesPerVertexBuffer
)
	: densityTexture(&densityTexture),
	  normalAmboTexture(&normalAmboTexture),
	  bytesPerVertexBuffer(bytesPerVertexBuffer),
	  wsMinVertexPos(vec3(0.0f)),
	  wsExtents(vec3(1.0f)),
	  transformFeedbackObj(0),
	  vbo_positions(0),
	  vbo_normals(0),
	  isEmpty(false),
	  processed(false),
	  numVertices(-1)
{
	glGenTransformFeedbacks(1, &transformFeedbackObj);
	glGenQueries(1, &query_feedbackPrimitivesWritten);
}

//---------------------------------------------------------------------------------------
TerrainBlock::~TerrainBlock() {
	glDeleteTransformFeedbacks(1, &transformFeedbackObj);
}

//---------------------------------------------------------------------------------------
GLuint TerrainBlock::getPositionVertexBuffer() const {
	return vbo_positions;
}

//---------------------------------------------------------------------------------------
GLuint TerrainBlock::getNormalVertexBuffer() const {
	return vbo_normals;
}

//---------------------------------------------------------------------------------------
GLuint TerrainBlock::getTransformFeedbackObj() const {
	return transformFeedbackObj;
}

//---------------------------------------------------------------------------------------
const Synergy::Texture3D & TerrainBlock::getDensityTexture() const {
	return *densityTexture;
}

//---------------------------------------------------------------------------------------
const Synergy::Texture3D & TerrainBlock::getNormalAmboTexture() const {
	return *normalAmboTexture;
}

//---------------------------------------------------------------------------------------
GLuint TerrainBlock::getBytesPerVertexBuffer() const {
	return bytesPerVertexBuffer;
}

//---------------------------------------------------------------------------------------
glm::vec3 TerrainBlock::getMinVertexPos() const {
	return wsMinVertexPos;
}

//---------------------------------------------------------------------------------------
GLuint TerrainBlock::getFeedbackPrimitivesWrittenQuery() const {
	return query_feedbackPrimitivesWritten;
}

//---------------------------------------------------------------------------------------
GLsizei TerrainBlock::getNumVertices() const {
	if (numVertices == -1) {
		GLuint vertexCount;

		// TODO Dustin - Caution, this causes a CPU stall until query result is ready on GPU.
		glGetQueryObjectuiv(query_feedbackPrimitivesWritten,
				GL_QUERY_RESULT, &vertexCount);
		glDeleteQueries(1, &query_feedbackPrimitivesWritten);

		numVertices = GLsizei(vertexCount);
	}

	return numVertices;
}
