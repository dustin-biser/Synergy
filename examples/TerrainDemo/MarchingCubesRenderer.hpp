/**
* @brief MarchingCubesRenderer
*
*/

#pragma once

#include <Synergy/Synergy.hpp>

const GLuint position_attrib_index = 0;
const GLuint normal_attrib_index = 1;

//-- Streams for transform feedback buffers
const GLuint streamIndex_wsPositions = 0;
const GLuint streamIndex_wsNormals = 1;

//-- Texture Unit Offsets:
const GLint densityGrid_texUnitOffset = 0;
const GLint triTable_texUnitOffset = 1;
const GLint normalAmbo_texUnitOffset = 2;



/**
* Given a 3D density texture density of dimension (gridWidth, gridHeight, gridDepth), this
* class will run the Marching Cubes algorithm which will generate
* (gridWidth-1)*(gridHeight-1)*(gridDepth-1) voxels with density values at the corner of
* each voxel.  From there triangles will be generated from voxel edge crossing of the
* iso-surface.
*/
class MarchingCubesRenderer {
    

public:
    MarchingCubesRenderer(Synergy::uint32 gridWidth,
                          Synergy::uint32 gridHeight,
                          Synergy::uint32 gridDepth);

    ~MarchingCubesRenderer();

    void render(const Synergy::Camera & camera,
                const Synergy::Texture3D & densityGrid_texture3d,
                Synergy::float32 isoSurfaceValue);

private:
    Synergy::float32 gridWidth;
    Synergy::float32 gridHeight;
    Synergy::float32 gridDepth;
    GLsizei numVoxelsPerLayer;
	GLsizei numVoxels;
    GLsizei transformFeedbackBufferSize;
    GLuint sampler_densityGrid;

	// 3D texture for storing normals and ambient occlusion factor.
	// Texture components correspond to the following:
	// RGB: surface normal.xyz (gradient of density texture)
	// A: ambient occlusion factor.
	Synergy::Texture3D normalAmbo_texture3d;

    //-- Shaders:
    Synergy::ShaderProgram shader_genIsoSurface;
    Synergy::ShaderProgram shader_renderIsoSurface;
    Synergy::ShaderProgram shader_voxelEdges;
	Synergy::ShaderProgram shader_computeNormalAmbo;


    //-- Vertex Array Objects:
    GLuint vao_voxelData; // For generating MC voxel data.
    GLuint vao_isoSurfaceTriangles; // For isosurface triangles rendering.
    GLuint vao_voxelEdgeLines; // For voxel edge line rendering.

	// Junk vao for processing normalAmbo_texture3d within
	// computeNormalAmboTexture() using no VBO data.
	GLuint junkVao;

    //-- Vertex Buffers:
    GLuint vbo_voxelIndices; // Voxel indices within parent block.
    GLuint streamBuffer_wsPositions; // For transform feedback.
    GLuint streamBuffer_wsNormals; // For transform feedback.
    GLuint voxelEdges_vertexBuffer; // Vertex data for voxel edge lines.
    GLuint voxelEdges_indexBuffer; // Indices for voxel edge lines.


    // Table for mapping mc_case to edges cut by isosurface.
    GLuint triTable_texture2d;

    // Used for gathering transform feedback primitive counts written.
    GLuint transformFeedbackObj;

	GLuint framebuffer;


	//-- Initialization Methods:
    void setupVoxelVboPositionData();
    void setupVoxelDataVao();
    void setupShaders();
    void setShaderUniforms();
    void uploadShaderUniformArrays();
    void setupSamplerObject();
    void setupTransformFeedback();
    void generateTriTableTexture();
    void setupVaoForIsoSurfaceTriangles();
    void setupVoxelEdgesVao();
    void setupVoxelEdgesVertexBuffer();

	void allocateNormalAmboTextureStorage();

	void initFramebufferWithColorAttachment (
			const Synergy::Texture3D & texture
	);

    void updateShaderUniforms(
		    const Synergy::Camera & camera
    );

    void generateIsoSurfaceTriangles(
		    const Synergy::Texture3D & densityGrid,
		    float isoSurfaceValue
    );

    void renderIsoSurface(
		    const Synergy::Camera & camera
    );

    void renderVoxelEdges (
		    const Synergy::Camera & camera
    );

	void computeNormalAmboTexture(
			const Synergy::Texture3D &densityGrid_texture3d
	);


    // TODO Dustin - remove after testing:
    void inspectTransformFeedbackBuffer();
	void inspectTextureData(
			const Synergy::Texture3D & texture,
			Synergy::uint32 numColorComponents
	);
	GLuint streamBuffer_debugOut; // For transform feedback.

};