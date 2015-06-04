// MarchingCubes.gs
#version 410

layout(points) in;

layout(points, max_vertices = 30) out; // 15 vertices and 15 normals

// World-space voxel size in each dimension xyz.
uniform vec3 voxelDim;

// World-space position of parent Block's min vertex.
uniform vec3 wsParentBlockPos;

uniform sampler3D densityGrid;
uniform sampler3D normalAmbo;

in vsOutGsIn {
	vec3 voxelIndex; // Voxel's index within parent Block.
	vec3 uvw; // Texture coordinate for voxel's min vertex.
	vec3 texStep; // uvw step size in texture space.
	vec4 f0123; // Density values of bottom 4 voxel vertices
	vec4 f4567; // Density values of top 4 voxel vertices
	uint mc_case; // 0-255, triTable case.
	float isoValue; // iso-surface value.
} gs_in[];


// Number of edges per voxel.
const int numEdges = 12;

// Total number of marching cube cases.
const int numCases = 256;

// Maps mc_case to a set of 5 edge number triplets (RGB) that the isosurface intersects.
// If a set starts with -1, then there are no more intersecting edges for the case.
uniform isampler2D triTable;

// Maps mc_case to number of triangles to output.  Range [0..5]
uniform uint case_to_numTriangles[numCases];

// Assume directed edges which start on a vertexA and end at a vertexB.

// Position within voxel of edge's vertexA, with vertex0 as origin.
uniform ivec3 edge_start[numEdges]; // .xyz in {0,1}

// Edge direction from vertexA to vertexB
uniform ivec3 edge_dir[numEdges]; // .xyz in {-1,0,1}


// Given an edge number, returns 1 at the vertex component location corresponding to
// edge's starting vertexA, 0 otherwise.
uniform vec4 cornerAmask0123[numEdges]; // .xyzw corresponds to vertex 0,1,2,3.
uniform vec4 cornerAmask4567[numEdges]; // .xyzw corresponds to vertex 4,5,6,7.

// Given an edge number, returns 1 at the vertex component location corresponding to
// edge's ending vertexB, 0 otherwise.
uniform vec4 cornerBmask0123[numEdges]; // .xyzw corresponds to vertex 0,1,2,3.
uniform vec4 cornerBmask4567[numEdges]; // .xyzw corresponds to vertex 4,5,6,7.

layout (stream = 0) out vec3 outWsPosition;
layout (stream = 1) out vec3 outWsNormal;

/////////////////////////////////////////////
// TODO Dustin - Remove after testing:
//layout (stream = 2) out vec3 debugStream;
/////////////////////////////////////////////


// Converts texture coordinates 'ts' to world space coordinates
vec3 textureSpaceToWorldSpace(vec3 ts) {
	return vec3(ts.x, ts.z, -ts.y);
}

void placeVertOnEdge(in int edgeNum, out vec3 vertexPosition) {

	// Obtain value at vertex A.
	float aValue = dot(cornerAmask0123[edgeNum], gs_in[0].f0123) +
	               dot(cornerAmask4567[edgeNum], gs_in[0].f4567);

	// Obtain value at vertex B.
	float bValue = dot(cornerBmask0123[edgeNum], gs_in[0].f0123) +
	               dot(cornerBmask4567[edgeNum], gs_in[0].f4567);

	// Distance along edge for the isosurface crossing.
	float t = (gs_in[0].isoValue - aValue) / (bValue - aValue); // t is in [0,1]

	vec3 pos_within_cell = edge_start[edgeNum] + t * edge_dir[edgeNum];

	// World-space position of voxel's minimum vertex.
	vec3 wsMinVertexPos =
			wsParentBlockPos + textureSpaceToWorldSpace(gs_in[0].voxelIndex) * voxelDim;

	vertexPosition = wsMinVertexPos + pos_within_cell * voxelDim;


	//-- Output stream vertex position:
	outWsPosition = vertexPosition;
	EmitStreamVertex(0);
	EndStreamPrimitive(0);

	//-- Compute interpolated normal from normalAmbo texture:
	{
		vec3 edgeDir = edge_dir[edgeNum];
		vec3 edgeStart = edge_start[edgeNum];

        // Convert from ws to texture space:
		vec3 texEdge_start = vec3(edgeStart.x, -edgeStart.z, edgeStart.y);
		vec3 texEdge_dir = vec3(edgeDir.x, -edgeDir.z, edgeDir.y);

		vec3 offset = texEdge_start + t * texEdge_dir; // in [0,1]

		vec3 uvw = gs_in[0].uvw + offset * gs_in[0].texStep;
		outWsNormal = texture(normalAmbo, uvw).rgb;

		//-- Output stream vertex normal:
		EmitStreamVertex(1);
		EndStreamPrimitive(1);
	}
}

void main() {
	uint mc_case = gs_in[0].mc_case;
	uint numTriangles = case_to_numTriangles[mc_case];

	for(int i = 0; i < numTriangles; ++i) {
		ivec3 edges = texelFetch(triTable, ivec2(i, mc_case), 0).rgb;
		vec3 vertexPosition[3];

		// Flip vertex winding so triangle's frontface is facing
		// form rock(+) towards air(-):
		placeVertOnEdge(edges[0], vertexPosition[0]);
		placeVertOnEdge(edges[2], vertexPosition[2]);
		placeVertOnEdge(edges[1], vertexPosition[1]);
	}
}
