#include "MarchingCubesSurfacePolygonizer.hpp"
#include "TerrainBlock.hpp"
#include <glm/gtx/transform.hpp>

#include <vector>
using std::vector;

using namespace Synergy;
using namespace glm;


//---------------------------------------------------------------------------------------
MarchingCubesSurfacePolygonizer::MarchingCubesSurfacePolygonizer (
		const uvec3 & densityGridDimensions
)
    : gridWidth(densityGridDimensions.x),
      gridHeight(densityGridDimensions.y),
      gridDepth(densityGridDimensions.z)
{
    setupShaders();
	generateTriTableTexture();
    setShaderUniforms();
	uploadShaderUniformArrays();

	setupVoxelVboPositionData();
	setupVoxelDataVao();

    setupSamplerObject();
}

//---------------------------------------------------------------------------------------
MarchingCubesSurfacePolygonizer::~MarchingCubesSurfacePolygonizer() {

}

//---------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::setupShaders() {
	shader_genIsoSurface.generateProgramObject();
	shader_genIsoSurface.attachVertexShader("shaders/MarchingCubes.vs");
	shader_genIsoSurface.attachGeometryShader("shaders/MarchingCubes.gs");

	// Link outWsPosition to stream index 0.
	// Link outWsNormal to stream index 1.
	const GLchar * feedbackVaryings[] = {"outWsPosition", "outWsNormal"};
	glTransformFeedbackVaryings(shader_genIsoSurface.getProgramObject(), 2,
			feedbackVaryings, GL_SEPARATE_ATTRIBS);

	shader_genIsoSurface.link();
}

//----------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::setShaderUniforms() {
	shader_genIsoSurface.setUniform("isoSurfaceValue", 0.0f);

	float inv_gridWidth = 1.0f / gridWidth;
	float inv_gridHeight = 1.0f / gridHeight;
	float inv_gridDepth = 1.0f / gridDepth;

    shader_genIsoSurface.setUniform("densityGrid", density_texUnitOffset);
	shader_genIsoSurface.setUniform("normalAmbo", normalAmbo_texUnitOffset);
	shader_genIsoSurface.setUniform("triTable", triTable_texUnitOffset);
    shader_genIsoSurface.setUniform("inv_gridWidth", inv_gridWidth);
    shader_genIsoSurface.setUniform("inv_gridHeight", inv_gridHeight);
    shader_genIsoSurface.setUniform("inv_gridDepth", inv_gridDepth);
	shader_genIsoSurface.setUniform("wsParentBlockPos", vec3(0.0f));

	// Voxel dimensions
	// Convert from grid-space to world-space:
	vec3 voxelDim = vec3(1.0f / (gridWidth - 1.0f),
	                       1.0f / (gridDepth - 1.0f),
	                       1.0f / (gridHeight - 1.0f));

	shader_genIsoSurface.setUniform("voxelDim", voxelDim);
	shader_genIsoSurface.setUniform("inv_gridDepth", 1.0f/gridDepth);
}


//----------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::uploadShaderUniformArrays() {
	uint32 case_to_numTriangles[256] = {
			0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 2, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3,
			3, 4, 3, 4, 4, 3, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 2,
			3, 4, 4, 3, 3, 4, 4, 3, 4, 5, 5, 2, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4,
			4, 3, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 4, 2, 3, 3, 4, 3, 4, 2, 3,
			3, 4, 4, 5, 4, 5, 3, 2, 3, 4, 4, 3, 4, 5, 3, 2, 4, 5, 5, 4, 5, 2, 4, 1, 1, 2,
			2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 4, 3, 4, 4, 5, 3, 2, 4, 3,
			4, 3, 5, 2, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 4, 3, 4, 4, 3, 4, 5,
			5, 4, 4, 3, 5, 2, 5, 4, 2, 1, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 2, 3, 3, 2,
			3, 4, 4, 5, 4, 5, 5, 2, 4, 3, 5, 4, 3, 2, 4, 1, 3, 4, 4, 5, 4, 5, 3, 4, 4, 5,
			5, 2, 3, 4, 2, 1, 2, 3, 3, 2, 3, 4, 2, 1, 3, 2, 4, 1, 2, 1, 1, 0
	};

	GLint case_to_numTriangles_location =
			shader_genIsoSurface.getUniformLocation("case_to_numTriangles");
	glUseProgram(shader_genIsoSurface.getProgramObject());
	glUniform1uiv(case_to_numTriangles_location, 256, case_to_numTriangles);
	CHECK_GL_ERRORS;



	// Position within voxel of edge's starting vertexA, with vertex0 as origin.
	int edge_start[12 * 3] = {
			          // Edge Number
			0, 0, 0,  // 0
			1, 0, 0,  // 1
			1, 0, -1, // 2
			0, 0, -1, // 3
			0, 1, 0,  // 4
			1, 1, 0,  // 5
			1, 1, -1, // 6
			0, 1, -1, // 7
			0, 0, 0,  // 8
			1, 0, 0,  // 9
			1, 0, -1, // 10
			0, 0, -1  // 11
	};

	GLint edge_start_location = shader_genIsoSurface.getUniformLocation("edge_start");
	glUseProgram(shader_genIsoSurface.getProgramObject());
	glUniform3iv(edge_start_location, 12, edge_start);
	CHECK_GL_ERRORS;



	// Edge direction from vertexA to vertexB
	int edge_dir[12 * 3] = {
					  // Edge Number
			1, 0, 0,  // 0
			0, 0, -1, // 1
			-1, 0, 0, // 2
			0, 0, 1,  // 3
			1, 0, 0,  // 4
			0, 0, -1, // 5
			-1, 0, 0, // 6
			0, 0, 1,  // 7
			0, 1, 0,  // 8
			0, 1, 0,  // 9
			0, 1, 0,  // 10
			0, 1, 0   // 11
	};

	GLint edge_dir_location = shader_genIsoSurface.getUniformLocation("edge_dir");
	glUseProgram(shader_genIsoSurface.getProgramObject());
	glUniform3iv(edge_dir_location, 12, edge_dir);
	CHECK_GL_ERRORS;

	
	
	// Given an edge number, returns 1 at the vertex component location corresponding to
	// edge's starting vertexA.
	// .xyzw corresponds to vertex 0,1,2,3.
	float32 cornerAmask0123[12 * 4] = {
			1, 0, 0, 0,     // edge 0, starts at vertex 0
			0, 1, 0, 0,     // edge 1, starts at vertex 1
			0, 0, 1, 0,     // edge 2, starts at vertex 2
			1, 0, 0, 0,     // edge 3, starts at vertex 0

			0, 0, 0, 0,     // edge 4, starts at vertex 4
			0, 0, 0, 0,     // edge 5, starts at vertex 5
			0, 0, 0, 0,     // edge 6, starts at vertex 6
			0, 0, 0, 0,     // edge 7, starts at vertex 4

			1, 0, 0, 0,     // edge 8, starts at vertex 0
			0, 1, 0, 0,     // edge 9, starts at vertex 1
			0, 0, 1, 0,     // edge 10, starts at vertex 2
			0, 0, 0, 1,     // edge 11, starts at vertex 3
	};
	
	GLint cornerAmask0123_location = shader_genIsoSurface.getUniformLocation("cornerAmask0123");
	glUseProgram(shader_genIsoSurface.getProgramObject());
	glUniform4fv(cornerAmask0123_location, 12, cornerAmask0123);
	CHECK_GL_ERRORS;


	// Given an edge number, returns 1 at the vertex component location corresponding to
	// edge's starting vertexA.
	// .xyzw corresponds to vertex 4,5,6,7.
	float32 cornerAmask4567[12 * 4] = {
			0, 0, 0, 0,     // edge 0, starts at vertex 0
			0, 0, 0, 0,     // edge 1, starts at vertex 1
			0, 0, 0, 0,     // edge 2, starts at vertex 2
			0, 0, 0, 0,     // edge 3, starts at vertex 3

			1, 0, 0, 0,     // edge 4, starts at vertex 4
			0, 1, 0, 0,     // edge 5, starts at vertex 5
			0, 0, 1, 0,     // edge 6, starts at vertex 6
			1, 0, 0, 0,     // edge 7, starts at vertex 4

			0, 0, 0, 0,     // edge 8, starts at vertex 0
			0, 0, 0, 0,     // edge 9, starts at vertex 1
			0, 0, 0, 0,     // edge 10, starts at vertex 2
			0, 0, 0, 0,     // edge 11, starts at vertex 3
	};

	GLint cornerAmask4567_location = shader_genIsoSurface.getUniformLocation("cornerAmask4567");
	glUseProgram(shader_genIsoSurface.getProgramObject());
	glUniform4fv(cornerAmask4567_location, 12, cornerAmask4567);
	CHECK_GL_ERRORS;



	// Given an edge number, returns 1 at the vertex component location corresponding to
	// edge's ending vertexB.
	// .xyzw corresponds to vertex 0,1,2,3.
	float32 cornerBmask0123[12 * 4] = {
			0, 1, 0, 0,     // edge 0, ends at vertex 1
			0, 0, 1, 0,     // edge 1, ends at vertex 2
			0, 0, 0, 1,     // edge 2, ends at vertex 3
			0, 0, 0, 1,     // edge 3, ends at vertex 3

			0, 0, 0, 0,     // edge 4, ends at vertex 5
			0, 0, 0, 0,     // edge 5, ends at vertex 6
			0, 0, 0, 0,     // edge 6, ends at vertex 7
			0, 0, 0, 0,     // edge 7, ends at vertex 7

			0, 0, 0, 0,     // edge 8, ends at vertex 4
			0, 0, 0, 0,     // edge 9, ends at vertex 5
			0, 0, 0, 0,     // edge 10, ends at vertex 6
			0, 0, 0, 0,     // edge 11, ends at vertex 7
	};

	GLint cornerBmask0123_location = shader_genIsoSurface.getUniformLocation("cornerBmask0123");
	glUseProgram(shader_genIsoSurface.getProgramObject());
	glUniform4fv(cornerBmask0123_location, 12, cornerBmask0123);
	CHECK_GL_ERRORS;



	// Given an edge number, returns 1 at the vertex component location corresponding to
	// edge's ending vertexB.
	// .xyzw corresponds to vertex 4,5,6,7.
	float32 cornerBmask4567[12 * 4] = {
			0, 0, 0, 0,     // edge 0, ends at vertex 1
			0, 0, 0, 0,     // edge 1, ends at vertex 2
			0, 0, 0, 0,     // edge 2, ends at vertex 3
			0, 0, 0, 0,     // edge 3, ends at vertex 3

			0, 1, 0, 0,     // edge 4, ends at vertex 5
			0, 0, 1, 0,     // edge 5, ends at vertex 6
			0, 0, 0, 1,     // edge 6, ends at vertex 7
			0, 0, 0, 1,     // edge 7, ends at vertex 7

			1, 0, 0, 0,     // edge 8, ends at vertex 4
			0, 1, 0, 0,     // edge 9, ends at vertex 5
			0, 0, 1, 0,     // edge 10, ends at vertex 6
			0, 0, 0, 1,     // edge 11, ends at vertex 7
	};

	GLint cornerBmask4567_location = shader_genIsoSurface.getUniformLocation("cornerBmask4567");
	glUseProgram(shader_genIsoSurface.getProgramObject());
	glUniform4fv(cornerBmask4567_location, 12, cornerBmask4567);
	CHECK_GL_ERRORS;


    glUseProgram(0);
    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::generateTriTableTexture() {
	// Maps mc_case to sets of 3 cut edges.
	int triTable[256 * 15] = {
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1,
			3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1,
			3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1,
			3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1,
			9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1,
			1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1,
			9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1,
			2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1,
			8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1,
			9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1,
			4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1,
			3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1,
			1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1,
			4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1,
			4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1,
			9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1,
			1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1,
			5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1,
			2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1,
			9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1,
			0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1,
			2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1,
			10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1,
			4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1,
			5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1,
			5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1,
			9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1,
			0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1,
			1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1,
			10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1,
			8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1,
			2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1,
			7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1,
			9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1,
			2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1,
			11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1,
			9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1,
			5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0,
			11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0,
			11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1,
			1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1,
			9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1,
			5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1,
			2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1,
			0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1,
			5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1,
			6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1,
			0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1,
			3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1,
			6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1,
			5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1,
			1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1,
			10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1,
			6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1,
			1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1,
			8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1,
			7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9,
			3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1,
			5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1,
			0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1,
			9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6,
			8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1,
			5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11,
			0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7,
			6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1,
			10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1,
			10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1,
			8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1,
			1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1,
			3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1,
			0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1,
			10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1,
			0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1,
			3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1,
			6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1,
			9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1,
			8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1,
			3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1,
			6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1,
			0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1,
			10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1,
			10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1,
			1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1,
			2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9,
			7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1,
			7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1,
			2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7,
			1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11,
			11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1,
			8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6,
			0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1,
			7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1,
			10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1,
			2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1,
			6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1,
			7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1,
			2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1,
			1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1,
			10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1,
			10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1,
			0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1,
			7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1,
			6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1,
			8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1,
			9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1,
			6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1,
			1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1,
			4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1,
			10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3,
			8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1,
			0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1,
			1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1,
			8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1,
			10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1,
			4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3,
			10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1,
			5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1,
			11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1,
			9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1,
			6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1,
			7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1,
			3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6,
			7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1,
			9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1,
			3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1,
			6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8,
			9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1,
			1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4,
			4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10,
			7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1,
			6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1,
			3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1,
			0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1,
			6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1,
			1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1,
			0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10,
			11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5,
			6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1,
			5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1,
			9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1,
			1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8,
			1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6,
			10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1,
			0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1,
			5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1,
			10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1,
			11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1,
			0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1,
			9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1,
			7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2,
			2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1,
			8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1,
			9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1,
			9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2,
			1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1,
			9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1,
			9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1,
			5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1,
			0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1,
			10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4,
			2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1,
			0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11,
			0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5,
			9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1,
			5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1,
			3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9,
			5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1,
			8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1,
			0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1,
			9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1,
			0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1,
			1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1,
			3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4,
			4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1,
			9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3,
			11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1,
			11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1,
			2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1,
			9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7,
			3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10,
			1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1,
			4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1,
			4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1,
			0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1,
			3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1,
			3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1,
			0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1,
			9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1,
			1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
			-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
	};

	glGenTextures(1, &triTable_texture2d);
	glBindTexture(GL_TEXTURE_2D, triTable_texture2d);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8I, 5 /*width*/, 256 /*height*/,
			0, GL_RGB_INTEGER, GL_INT, triTable);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// border color defaults to 0.

	glBindTexture(GL_TEXTURE_2D, 0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::setupVoxelVboPositionData() {
    glGenBuffers(1, &vbo_voxelIndices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_voxelIndices);

    // Every 2 floats will represent the position of minimum vertex of voxel within
	// parent Block.  Only storing vec2's since layer can be computed automatically via
	// gl_InstanceID in the VS.
    vector<vec2> gridPositions;

	// position.x in [0, gridWidth - 1]
	// position.y in [0, gridHeight - 1]
    vec2 position(0.0f);

    for (int32 j = 0; j < gridHeight-1; ++j) {
		for (int32 i = 0; i < gridWidth-1; ++i) {

            position.x = i;
            position.y = j;

            gridPositions.push_back(position);
        }
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2)* gridPositions.size(), gridPositions.data(),
            GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Set Vao Data Bindings
void MarchingCubesSurfacePolygonizer::setupVoxelDataVao() {
    glGenVertexArrays(1, &vao_voxelData);
    glBindVertexArray(vao_voxelData);

	const GLuint position_attrib_index = 0;

    glEnableVertexAttribArray(position_attrib_index);

    //-- voxel minimum vertex positions:
    {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_voxelIndices);

        int32 elementsPerVertex = 2;
        int32 stride = 0;
        int32 offsetToFirstElement = 0;
        glVertexAttribPointer(position_attrib_index, elementsPerVertex, GL_FLOAT,
                GL_FALSE, stride, reinterpret_cast<void *>(offsetToFirstElement));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERRORS;
    }

    //-- Restore Defaults:
    glBindVertexArray(0);
    CHECK_GL_ERRORS;
}


//----------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::setupSamplerObject() {
    glGenSamplers(1, &sampler_densityGrid);
    glSamplerParameterf(sampler_densityGrid, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glSamplerParameterf(sampler_densityGrid, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::generateSurface (
		TerrainBlock & block
){
	setTransformFeedbackStreamBuffers(block);
	generateSurfaceVertices(block);

//	#ifdef DEBUG
//		inspectTransformFeedbackBuffer(block);
//	#endif
}

//----------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::setTransformFeedbackStreamBuffers(
		const TerrainBlock & block
) {
	const GLuint streamIndex_wsPositions = 0;
	const GLuint streamIndex_wsNormals = 1;

	//-- Setup transform feedback stream index binding points:
	{
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, block.getTransformFeedbackObj());

		// vertex positions:
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,
				streamIndex_wsPositions, block.getPositionVertexBuffer());

		// vertex normals:
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,
				streamIndex_wsNormals, block.getNormalVertexBuffer());

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		CHECK_GL_ERRORS;
	}
}
//----------------------------------------------------------------------------------------
template<typename T>
static void zeroOutArray(T * data, uint32 numElements) {
	for(uint32 i = 0; i < numElements; ++i) {
		data[i] = T(0);
	}
}

//----------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::inspectTransformFeedbackBuffer (
		const TerrainBlock & block
) {
	// Empty out GL pipeline so that stream buffers have data.
	glFlush();
	glFinish();

    GLsizei numElements =  block.getBytesPerVertexBuffer() / sizeof(GLfloat);
    GLfloat * stream0Data = new GLfloat[numElements];
	GLfloat * stream1Data = new GLfloat[numElements];

	zeroOutArray<GLfloat>(stream0Data, numElements);
	zeroOutArray<GLfloat>(stream1Data, numElements);

	glBindBuffer(GL_ARRAY_BUFFER, block.getPositionVertexBuffer());
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, block.getBytesPerVertexBuffer(), stream0Data);

	glBindBuffer(GL_ARRAY_BUFFER, block.getNormalVertexBuffer());
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, block.getBytesPerVertexBuffer(), stream1Data);

	delete [] stream0Data;
	delete [] stream1Data;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void MarchingCubesSurfacePolygonizer::generateSurfaceVertices(
		const TerrainBlock & block
) {
	// Prevent rasterization.
	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(vao_voxelData);

	glActiveTexture(GL_TEXTURE0 + density_texUnitOffset);
	block.getDensityTexture().bind();
	glBindSampler(density_texUnitOffset, sampler_densityGrid);

	glActiveTexture(GL_TEXTURE0 + normalAmbo_texUnitOffset);
	block.getNormalAmboTexture().bind();

	glActiveTexture(GL_TEXTURE0 + triTable_texUnitOffset);
	glBindTexture(GL_TEXTURE_2D, triTable_texture2d);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, block.getTransformFeedbackObj());

	GLsizei numVoxelsPerLayer = (gridWidth - 1.0f) * (gridHeight - 1.0f);
	GLsizei numInstances = gridDepth - 1.0f;

	shader_genIsoSurface.enable();
		glBeginTransformFeedback(GL_POINTS);

		// Each instance is a 2D grid of points.
		glDrawArraysInstanced(GL_POINTS, 0, numVoxelsPerLayer, numInstances);

		glEndTransformFeedback();
	shader_genIsoSurface.disable();

	//-- Restore defaults:
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_3D, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(density_texUnitOffset, 0);
	glDisable(GL_RASTERIZER_DISCARD);
	CHECK_GL_ERRORS;
}



