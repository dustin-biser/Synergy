#include "RockDensityGenerator.hpp"
#include "TerrainBlock.hpp"

using namespace Synergy;


// TODO Dustin - Refactor this method to use a share pass to compute rock density
// based on various frequencies of PerlinNoise texture.
void RockDensityGenerator::generateRockDensity(
		TerrainBlock & block
) {
	const Texture3D & densityTexture = *(block.densityTexture);

	const int width = densityTexture.width();
	const int height = densityTexture.height();
	const int depth = densityTexture.depth();

	float32 * data = new float32[depth * height * width];

	float isoValue = 0;

	for(int k(0); k < depth; ++k) {
		for(int j(0); j < height; ++j) {
			for(int i(0); i < width; ++i) {
				// Set all values below isosurface isoValue
				data[k * (height * width) + (j * width) + i] = isoValue - 1.0f;
			}
		}
	}

//	data[(0 * height * width) + (0 * width) + 0] = isoValue + 1;  // Vertex 0
//    data[(0 * height * width) + (0 * width) + 1] = isoValue + 1;  // Vertex 1
//    data[(0 * height * width) + (0 * width) + 2] = isoValue + 1;  // Vertex 2

//    data[(0 * height * width) + (1 * width) + 0] = isoValue + 1;  // Vertex 3
//    data[(0 * height * width) + (1 * width) + 1] = isoValue + 1;  // Vertex 4
//    data[(0 * height * width) + (1 * width) + 2] = isoValue + 1;  // Vertex 5

//    data[(0 * height * width) + (2 * width) + 0] = isoValue + 1;  // Vertex 6
//    data[(0 * height * width) + (2 * width) + 1] = isoValue + 1;  // Vertex 7
//	  data[(0 * height * width) + (2 * width) + 2] = isoValue + 1;  // Vertex 8

//    data[(1 * height * width) + (0 * width) + 0] = isoValue + 1; //Vertex 9
//    data[(1 * height * width) + (0 * width) + 1] = isoValue + 1; //Vertex 10
//    data[(1 * height * width) + (0 * width) + 2] = isoValue + 1; //Vertex 11

//    data[(1 * height * width) + (1 * width) + 0] = isoValue + 1; //Vertex 12
    data[(1 * height * width) + (1 * width) + 1] = isoValue + 1; //Vertex 13
//    data[(1 * height * width) + (1 * width) + 2] = isoValue + 1; //Vertex 14

//    data[(1 * height * width) + (2 * width) + 0] = isoValue + 1; //Vertex 15
//    data[(1 * height * width) + (2 * width) + 1] = isoValue + 1; //Vertex 16
//    data[(1 * height * width) + (2 * width) + 2] = isoValue + 1; //Vertex 17

//    data[(2 * height * width) + (0 * width) + 0] = isoValue + 1; //Vertex 18
//    data[(2 * height * width) + (0 * width) + 1] = isoValue + 1; //Vertex 19
//    data[(2 * height * width) + (0 * width) + 2] = isoValue + 1; //Vertex 20

//    data[(2 * height * width) + (1 * width) + 0] = isoValue + 1; //Vertex 21
//    data[(2 * height * width) + (1 * width) + 1] = isoValue + 1; //Vertex 22
//    data[(2 * height * width) + (1 * width) + 2] = isoValue + 1; //Vertex 23

//    data[(2 * height * width) + (2 * width) + 0] = isoValue + 1; //Vertex 24
//    data[(2 * height * width) + (2 * width) + 1] = isoValue + 1; //Vertex 25
//    data[(2 * height * width) + (2 * width) + 2] = isoValue + 1; //Vertex 26

	densityTexture.bind();
	// Copy texture into data's memory:
	glTexSubImage3D(densityTexture.type, 0, 0, 0, 0, width,
			height, depth, GL_RED, GL_FLOAT, data);
	densityTexture.unbind();


	delete [] data;
}
