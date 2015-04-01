/// TerainBlockGenerator

#pragma once

#include <map>
#include <Synergy/Synergy.hpp>

#include "VboMemoryPool.hpp"

// Forward Declaration
class TerrainBlock;


// Represents integer coordinates of a TerrainBlock's min vertex.
typedef long WorldSpaceIndex;

class TerrainBlockGenerator {

public:
	TerrainBlockGenerator(const glm::uvec3 & densityGridDimensions);

	~TerrainBlockGenerator();

	void queryVisibleBlocks (
			const Synergy::Camera & camera,
			std::map<WorldSpaceIndex, TerrainBlock *> & blockMap
	);

private:
	VboMemoryPool vboMemoryPool;

	Synergy::Texture3D densityTexture;
	Synergy::Texture3D normalAmboTexture;

	std::map<WorldSpaceIndex, TerrainBlock *> blockMap;

	Synergy::uint32 bytesPerVertexBuffer;

	void createTextureStorage(const glm::uvec3 & densityGridDimensions);

	void initVboMemoryPool(const glm::uvec3 & densityGridDimensions);

	glm::uint32 computeVertexBufferBytes(const glm::uvec3 & densityGridDimensions);

};