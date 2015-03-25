// LineRender.vs
#version 410

layout (location = 0) in vec3 v_position;

// Stored in the multiplication order (Proj * View)
uniform mat4 ViewProjMatrix;

// World-space position of parent block's minimum  vertex.
uniform vec3 wsBlockMinVertexPos;

uniform float numVoxelCols; // Number of voxel columns within grid.
uniform float numVoxelRows; // Number of voxel rows within grid.
uniform float numVoxelLayers; // Number of voxel layers within grid.

void main() {
	//-- Compute indices i,j,k of voxel within grid given only its gl_InstanceID.
		float gridSliceArea = numVoxelCols * numVoxelRows;
		float k = int(gl_InstanceID / gridSliceArea);
		float tmp = gl_InstanceID - gridSliceArea * k;
		float j = int(tmp / numVoxelCols);
		float i = tmp - numVoxelCols * j;

	// Minimum vertex position within parent block.  The voxel instance will be
	// translated into position based on this world-space offset.
	vec3 wsMinVertexPos = vec3(i / numVoxelCols,
							   j / numVoxelRows,
							   -k / numVoxelLayers);
    wsMinVertexPos += wsBlockMinVertexPos;
	mat4 translate = mat4(1.0);
	translate[3] = vec4(wsMinVertexPos, 1.0);

	vec3 scale = vec3(1.0 / numVoxelCols,
					  1.0 / numVoxelRows,
				      1.0 / numVoxelLayers);
	mat4 scaleMat4 = mat4(1.0);
	scaleMat4[0][0] = scale.x;
	scaleMat4[1][1] = scale.y;
	scaleMat4[2][2] = scale.y;


    gl_Position = ViewProjMatrix * translate * scaleMat4 * vec4(v_position, 1.0);
}
