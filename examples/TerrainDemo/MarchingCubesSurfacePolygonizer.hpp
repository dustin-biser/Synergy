/**
* @brief MarchingCubesRenderer
*
*/

#pragma once

#include <Synergy/Synergy.hpp>
#include "TerrainBlock.hpp"

// Forward Declaration
class TerrainBlock;



/**
* Given a 3D density texture of dimension (gridWidth, gridHeight, gridDepth), this
* class will run the Marching Cubes algorithm which will generate
* (gridWidth-1)*(gridHeight-1)*(gridDepth-1) voxels with density values at the corner of
* each voxel.  From there triangles will be generated from voxel edge crossing of the
* iso-surface.
*/
class MarchingCubesSurfacePolygonizer {

public:
    MarchingCubesSurfacePolygonizer(
		    const glm::uvec3 & densityGridDimensions
    );

    ~MarchingCubesSurfacePolygonizer();

    void generateSurface(TerrainBlock & block);


private:
	//-- Texture Units:
	const GLint density_texUnitOffset = 0;
	const GLint triTable_texUnitOffset = 1;
	const GLint normalAmbo_texUnitOffset = 2;

    Synergy::float32 gridWidth;
    Synergy::float32 gridHeight;
    Synergy::float32 gridDepth;
    GLuint sampler_densityGrid;

    //-- Shaders:
    Synergy::ShaderProgram shader_genIsoSurface;

    GLuint vao_voxelData; // For generating MC voxel data.
    GLuint vbo_voxelIndices; // Voxel indices within parent block.

    // Table for mapping mc_case to edges cut by isosurface.
    GLuint triTable_texture2d;


	//-- Initialization Methods:
    void setupVoxelVboPositionData();
    void setupVoxelDataVao();
    void setupShaders();
    void setShaderUniforms();
    void uploadShaderUniformArrays();
    void setupSamplerObject();
    void generateTriTableTexture();

	void setTransformFeedbackStreamBuffers(const TerrainBlock & block);

    void generateSurfaceVertices(const TerrainBlock & block);


    // TODO Dustin - remove after testing:
    GLuint vbo_debugStream;
	GLsizei vbo_debugStream_bytes;
    void inspectTransformFeedbackBuffer(const TerrainBlock & block);
	void allocateVboDebugStreamStorage();
	GLuint query;
};
