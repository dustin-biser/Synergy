#include "TerrainDemo.hpp"
#include "MarchingCubesRenderer.hpp"

using std::shared_ptr;

//----------------------------------------------------------------------------------------
int main() {
    shared_ptr<GlfwOpenGlWindow> demo =  TerrainDemo::getInstance();
    demo->create(kScreenWidth, kScreenHeight, "Marching Cubes Demo", 1/60.0f);

    return 0;
}

//---------------------------------------------------------------------------------------
shared_ptr<GlfwOpenGlWindow> TerrainDemo::getInstance() {
    static GlfwOpenGlWindow * instance = new TerrainDemo();
    if (p_instance == nullptr) {
        p_instance = shared_ptr<GlfwOpenGlWindow>(instance);
    }

    return p_instance;
}

//---------------------------------------------------------------------------------------
TerrainDemo::~TerrainDemo() {
    delete marchingCubesRenderer;
}


//---------------------------------------------------------------------------------------
void TerrainDemo::init() {
    marchingCubesRenderer =
            new MarchingCubesRenderer(kGridWidth, kGridHeight, kGridDepth);

    createTextureStorage();
    fillVolumeDensityTexture();
    setupCamera();


    glDisable(GL_CULL_FACE);

    //-- Setup depth testing:
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); // Allow writing to depth buffer.
    glDepthFunc(GL_LESS);
    glDepthRange(0.0f, 1.0f);
    glEnable(GL_DEPTH_CLAMP);

    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClearDepth(1.0f);

    CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void TerrainDemo::createTextureStorage() {
	Synergy::TextureSpec textureSpec;
	textureSpec.width = kGridWidth;
	textureSpec.height = kGridHeight;
	textureSpec.depth = kGridDepth;
	textureSpec.internalFormat = GL_RED;
	textureSpec.format = GL_RED;
	textureSpec.dataType = GL_FLOAT;
	volumeDensity.allocateStorage(textureSpec);
	volumeDensity.bind();
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	volumeDensity.unbind();
}

//---------------------------------------------------------------------------------------
void TerrainDemo::fillVolumeDensityTexture() {
	const int width = volumeDensity.width();
	const int height = volumeDensity.height();
	const int depth = volumeDensity.depth();

    float32 * data = new float32[depth * height * width];

	float value = kIsoSurfaceValue;

    for(int k(0); k < depth; ++k) {
        for(int j(0); j < height; ++j) {
            for(int i(0); i < width; ++i) {
	            // Set all values below isosurface value
                data[k * (height * width) + (j * width) + i] =
		                value - 1.0f;
            }
        }
    }

    data[(0 * height * width) + (0 * width) + 0] = value + 1;  // Vertex 0
    data[(0 * height * width) + (0 * width) + 1] = value + 1;  // Vertex 1
    data[(0 * height * width) + (0 * width) + 2] = value + 1;  // Vertex 2

//    data[(0 * height * width) + (1 * width) + 0] = value + 1;  // Vertex 3
//    data[(0 * height * width) + (1 * width) + 1] = value + 1;  // Vertex 4
//    data[(0 * height * width) + (1 * width) + 2] = value + 1;  // Vertex 5

//    data[(0 * height * width) + (2 * width) + 0] = value + 1;  // Vertex 6
//    data[(0 * height * width) + (2 * width) + 1] = value + 1;  // Vertex 7
//	  data[(0 * height * width) + (2 * width) + 2] = value + 1;  // Vertex 8

//    data[(1 * height * width) + (0 * width) + 0] = value + 1; //Vertex 9
//    data[(1 * height * width) + (0 * width) + 1] = value + 1; //Vertex 10
//    data[(1 * height * width) + (0 * width) + 2] = value + 1; //Vertex 11

//    data[(1 * height * width) + (1 * width) + 0] = value + 1; //Vertex 12
//    data[(1 * height * width) + (1 * width) + 1] = value + 1; //Vertex 13
//    data[(1 * height * width) + (1 * width) + 2] = value + 1; //Vertex 14

//    data[(1 * height * width) + (2 * width) + 0] = value + 1; //Vertex 15
//    data[(1 * height * width) + (2 * width) + 1] = value + 1; //Vertex 16
//    data[(1 * height * width) + (2 * width) + 2] = value + 1; //Vertex 17

//    data[(2 * height * width) + (0 * width) + 0] = value + 1; //Vertex 18
//    data[(2 * height * width) + (0 * width) + 1] = value + 1; //Vertex 19
//    data[(2 * height * width) + (0 * width) + 2] = value + 1; //Vertex 20

//    data[(2 * height * width) + (1 * width) + 0] = value + 1; //Vertex 21
//    data[(2 * height * width) + (1 * width) + 1] = value + 1; //Vertex 22
//    data[(2 * height * width) + (1 * width) + 2] = value + 1; //Vertex 23

//    data[(2 * height * width) + (2 * width) + 0] = value + 1; //Vertex 24
//    data[(2 * height * width) + (2 * width) + 1] = value + 1; //Vertex 25
//    data[(2 * height * width) + (2 * width) + 2] = value + 1; //Vertex 26


	volumeDensity.bind();
		// Copy texture into data's memory:
		glTexSubImage3D(volumeDensity.type, 0, 0, 0, 0, width,
				height, depth, GL_RED, GL_FLOAT, data);
	volumeDensity.unbind();


    delete [] data;
}

//---------------------------------------------------------------------------------------
void TerrainDemo::setupCamera() {
    camera.setNearZDistance(0.1f);
    camera.setPosition(vec3(1.8,1.2,1.2));
    camera.lookAt(vec3(0.5,0.5,-0.5));

    cameraController.setForwardScaleFactor(0.05f/kGridWidth);
    cameraController.setSideStrafeScaleFactor(0.05f/kGridWidth);
    cameraController.setUpScaleFactor(0.05f/kGridWidth);
}

//---------------------------------------------------------------------------------------
void TerrainDemo::logic() {

}

//---------------------------------------------------------------------------------------
void TerrainDemo::draw() {
    marchingCubesRenderer->render(camera, volumeDensity, kIsoSurfaceValue);
}

//---------------------------------------------------------------------------------------
void TerrainDemo::cleanup() {

}
