#include "TerrainBlockGenerator.hpp"
#include "TerrainBlock.hpp"
#include "VboMemoryPool.hpp"

using namespace std;
using namespace Synergy;
using namespace glm;


//---------------------------------------------------------------------------------------
TerrainBlockGenerator::TerrainBlockGenerator (
		const uvec3 & densityGridDimensions
) {
	createTextureStorage(densityGridDimensions);
	initVboMemoryPool(densityGridDimensions);

	// TODO Dustin - Refactor block creation so that all MaxTerrainBlocks are allocated
	// at once and reused during lifetime of TerrainBlockGenerator
	{
		for(int j(0); j < 10; ++j) {
		for(int i(0); i < 10; ++i) {
				TerrainBlock * block =
						new TerrainBlock(densityTexture, normalAmboTexture, bytesPerVertexBuffer);
				block->wsMinVertexPos = vec3(j,0,-i);

				vboMemoryPool.requestVertexBuffer(block->vbo_positions);
				vboMemoryPool.requestVertexBuffer(block->vbo_normals);

				// Insert into blockMap:
				WorldSpaceIndex wsIndex = i + j*10;
				blockMap[wsIndex] = block;
			}
		}
	}
}

//---------------------------------------------------------------------------------------
TerrainBlockGenerator::~TerrainBlockGenerator() {
	// Release all TerrainBlock memory:
	for(auto pair : blockMap) {
		TerrainBlock * block = pair.second;
		delete block;
	}
}

//---------------------------------------------------------------------------------------
uint32 TerrainBlockGenerator::computeVertexBufferBytes(
		const uvec3 & densityGridDimensions
) {
	uvec3 dim = densityGridDimensions - uvec3(1.0f);
	uint32 numVoxelsPerBlock = dim.x * dim.y * dim.z;
	uint32 maxTrianglesPerVoxel = 5;
	uint32 numVerticesPerTriangle = 3;
	uint32 numElementsPerVertex = 3;

	uint32 numVertexBufferBytes = 1;
	numVertexBufferBytes *= numVoxelsPerBlock;
	numVertexBufferBytes *= maxTrianglesPerVoxel;
	numVertexBufferBytes *= numVerticesPerTriangle;
	numVertexBufferBytes *= numElementsPerVertex;
	numVertexBufferBytes *= uint32(sizeof(float));

	return numVertexBufferBytes;
}

//---------------------------------------------------------------------------------------
void TerrainBlockGenerator::initVboMemoryPool (
		const uvec3 & densityGridDimension
) {
	bytesPerVertexBuffer = computeVertexBufferBytes(densityGridDimension);
	uint32 numVertexBuffers = 300;
	vboMemoryPool.allocateStorage(numVertexBuffers, bytesPerVertexBuffer);
}

//---------------------------------------------------------------------------------------
void TerrainBlockGenerator::queryVisibleBlocks (
		Camera const & camera,
		map<WorldSpaceIndex, TerrainBlock *> & blockMap
) {
	blockMap = this->blockMap;
}

//---------------------------------------------------------------------------------------
void TerrainBlockGenerator::createTextureStorage (
		const glm::uvec3 & densityGridDimensions
) {
	//-- Allocate storage for densityTexture:
	{
		Synergy::TextureSpec textureSpec;
		textureSpec.width = densityGridDimensions.x;
		textureSpec.height = densityGridDimensions.y;
		textureSpec.depth = densityGridDimensions.z;
		textureSpec.internalFormat = GL_R32F;
		textureSpec.format = GL_RED;
		textureSpec.dataType = GL_FLOAT;

		densityTexture.allocateStorage(textureSpec);

		densityTexture.bind();
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		densityTexture.unbind();
	}

	//-- Allocate storage for normalAmboTexture:
	{

		Synergy::TextureSpec textureSpec;
		textureSpec.width = densityGridDimensions.x;
		textureSpec.height = densityGridDimensions.y;
		textureSpec.depth = densityGridDimensions.z;
		textureSpec.internalFormat = GL_RGBA16F;
		textureSpec.format = GL_RGBA;
		textureSpec.dataType = GL_FLOAT;

		normalAmboTexture.allocateStorage(textureSpec);

		normalAmboTexture.bind();
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		normalAmboTexture.unbind();
	}
}

//---------------------------------------------------------------------------------------
const Synergy::Texture3D &TerrainBlockGenerator::getSharedNormalAmboTexture() {
	return normalAmboTexture;
}

//---------------------------------------------------------------------------------------
const Synergy::Texture3D &TerrainBlockGenerator::getSharedDensityTexture() {
	return densityTexture;
}
