/**
 * TerrainBlock
 */

#pragma once

#include <Synergy/Synergy.hpp>

// Forward Declarations
class TerrainBlockGenerator;
class RockDensityGenerator;
class LightingOven;


class TerrainBlock {
public:
	TerrainBlock();

	~TerrainBlock();

	TerrainBlock(
			const Synergy::Texture3D & densityTexture,
			const Synergy::Texture3D & normalAmboTexture,
			Synergy::uint32 bytesPerVertexBuffer
	);

	GLuint getPositionVertexBuffer() const;
	GLuint getNormalVertexBuffer() const;
	GLuint getTransformFeedbackObj() const;
	GLuint getBytesPerVertexBuffer() const;

	const Synergy::Texture3D & getDensityTexture() const;
	const Synergy::Texture3D & getNormalAmboTexture() const;

	bool processed;
	bool isEmpty;

private:
	friend class TerrainBlockGenerator;
	friend class RockDensityGenerator;
	friend class LightingOven;

	glm::vec3 wsMinVertexPos;
	glm::vec3 wsExtents;

	const Synergy::Texture3D * densityTexture;
	const Synergy::Texture3D * normalAmboTexture;

	GLuint transformFeedbackObj;
	GLuint vbo_positions;
	GLuint vbo_normals;
	Synergy::uint32 bytesPerVertexBuffer;
};