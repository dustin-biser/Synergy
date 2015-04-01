#include "TerrainBlock.hpp"

using namespace glm;



//---------------------------------------------------------------------------------------
TerrainBlock::TerrainBlock()
	: wsMinVertexPos(vec3(0)),
	  wsExtents(vec3(0)),
	  densityTexture(nullptr),
	  normalAmboTexture(nullptr),
	  transformFeedbackObj(0),
	  vbo_positions(0),
	  vbo_normals(0),
	  isEmpty(false),
      processed(false)
{

}

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
	  wsExtents(vec3(1.0f))
{
	glGenTransformFeedbacks(1, &transformFeedbackObj);
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
