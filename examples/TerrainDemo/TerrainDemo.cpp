#include "TerrainDemo.hpp"

#include "MarchingCubesSurfacePolygonizer.hpp"
#include "RockDensityGenerator.hpp"
#include "LightingOven.hpp"
#include "TerrainRenderer.hpp"
#include "Skybox.hpp"
#include "RenderTarget.hpp"
#include "PostProcessRenderer.hpp"

using std::shared_ptr;
using namespace glm;
using namespace Synergy;

//----------------------------------------------------------------------------------------
int main() {
    shared_ptr<GlfwOpenGlWindow> demo =  TerrainDemo::getInstance();
    demo->create(kScreenWidth, kScreenHeight, "Terrain Rendering Demo", 1.0f/60.0f);

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
TerrainDemo::TerrainDemo()
	: renderSkybox(false),
	  gammaCorrection(false)
{

}

//---------------------------------------------------------------------------------------
void TerrainDemo::init() {
	//-- Initialize subsystems:
	{
		uvec3 densityGridDimensions(kGridWidth, kGridHeight, kGridDepth);

		terrainBlockGenerator =
				new TerrainBlockGenerator(densityGridDimensions);

		rockDensityGenerator = new RockDensityGenerator();
		rockDensityGenerator->setTextureRenderTarget(
			terrainBlockGenerator->getSharedDensityTexture()
		);

		lightingOven = new LightingOven(densityGridDimensions);
		lightingOven->setTextureRenderTarget(
				terrainBlockGenerator->getSharedNormalAmboTexture()
		);

		terrainRenderer = new TerrainRenderer(densityGridDimensions);

		surfacePolygonizer =
				new MarchingCubesSurfacePolygonizer(densityGridDimensions);

		skybox = new Skybox();

		renderTarget = new RenderTarget(
				defaultFramebufferWidth(),
				defaultFramebufferHeight()
		);

		postProcessRenderer = new PostProcessRenderer();
	}

    setupCamera();
	setupGl();
	initSkyboxTextures();
}

//---------------------------------------------------------------------------------------
TerrainDemo::~TerrainDemo() {
	delete rockDensityGenerator;
	delete lightingOven;
	delete terrainRenderer;
	delete surfacePolygonizer;
	delete terrainBlockGenerator;
	delete skybox;
	delete renderTarget;
	delete postProcessRenderer;
}

//---------------------------------------------------------------------------------------
void TerrainDemo::setupGl() {

//	glEnable(GL_CULL_FACE);
//	glCullFace(GL_BACK);
//	glFrontFace(GL_CCW);

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
void TerrainDemo::setupCamera() {
    camera.setNearZDistance(0.1f);
	camera.setFarZDistance(100.0f);
    camera.setPosition(vec3(-1.0,1.0,1.0));
    camera.lookAt(vec3(0.5,0.5,-1.5));

    cameraController.setForwardScaleFactor(0.05f);
    cameraController.setSideStrafeScaleFactor(0.05f);
    cameraController.setUpScaleFactor(0.02f);
}

//---------------------------------------------------------------------------------------
void TerrainDemo::keyInput(int key, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_1:
				toggleTerrainNormalVisualization();
				break;

			case GLFW_KEY_2:
				toggleBlockEdgeVisualization();
		        break;

			case GLFW_KEY_3:
				toggleShowVoxels();
		        break;

			case GLFW_KEY_4:
				renderSkybox = !renderSkybox;
				break;

			case GLFW_KEY_5:
				gammaCorrection = !gammaCorrection;
		        break;

			default:
				break;
		}
	}

}

//---------------------------------------------------------------------------------------
void TerrainDemo::toggleTerrainNormalVisualization() {
	static bool visualizeNormals = true;
	if (visualizeNormals) {
		terrainRenderer->enableVisualizeNormals();
	} else {
		terrainRenderer->disableVisualizeNormals();
	}

	visualizeNormals = !visualizeNormals;
}

//---------------------------------------------------------------------------------------
void TerrainDemo::toggleShowVoxels() {
	static bool visualizeVoxels = true;
	if (visualizeVoxels) {
		terrainRenderer->enableVisualizeVoxels();
	} else {
		terrainRenderer->disableVisualizVoxels();
	}

	visualizeVoxels = !visualizeVoxels;

}

//---------------------------------------------------------------------------------------
void TerrainDemo::toggleBlockEdgeVisualization() {
	static bool visualizeVoxelEdges = false;
	if (visualizeVoxelEdges) {
		terrainRenderer->enableVisualizeBlocks();
	} else {
		terrainRenderer->disableVisualizeBlocks();
	}

	visualizeVoxelEdges = !visualizeVoxelEdges;
}

//---------------------------------------------------------------------------------------
void TerrainDemo::initSkyboxTextures() {
	const char * front =
			"textures/cube_maps/TropicalSunnyDay/TropicalSunnyDayFront512.png";
	const char * back =
			"textures/cube_maps/TropicalSunnyDay/TropicalSunnyDayBack512.png";
	const char * left =
			"textures/cube_maps/TropicalSunnyDay/TropicalSunnyDayLeft512.png";
	const char * right =
			"textures/cube_maps/TropicalSunnyDay/TropicalSunnyDayRight512.png";
	const char * up =
			"textures/cube_maps/TropicalSunnyDay/TropicalSunnyDayUp512.png";
	const char * down =
			"textures/cube_maps/TropicalSunnyDay/TropicalSunnyDayDown512.png";

	skybox->loadCubeMap(front, back, left, right, up, down);
}


//---------------------------------------------------------------------------------------
void TerrainDemo::logic() {
	renderTarget->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(renderSkybox) {
		skybox->render(camera, renderTarget);
	}

	terrainBlockGenerator->queryVisibleBlocks(camera, blockMap);

	for(auto & pair : blockMap) {
		TerrainBlock & block = *(pair.second);

		if(block.processed == false) {

			rockDensityGenerator->generateRockDensity(block);

			if (block.isEmpty) continue; // Skip rendering and goto next block.

			lightingOven->bakeNormals(block);
			lightingOven->bakeAmbientOcclusion(block);
			surfacePolygonizer->generateSurface(block);

			block.processed = true;
		}
	}


	for(auto & pair : blockMap) {
		TerrainBlock &block = *(pair.second);

		terrainRenderer->render(camera, block, renderTarget);
	}
}

//---------------------------------------------------------------------------------------
void TerrainDemo::draw() {
	if(gammaCorrection) {
		postProcessRenderer->applyGammaCorrection(*renderTarget, 1.4f);
	} else {
		postProcessRenderer->render(*renderTarget);
	}
}

//---------------------------------------------------------------------------------------
void TerrainDemo::cleanup() {

}

//---------------------------------------------------------------------------------------
void TerrainDemo::inspectTextureData(
		const Texture3D & texture
) {
	uint32 width = texture.width();
	uint32 height = texture.height();
	uint32 depth = texture.depth();
	uint32 numColorComponents = 1;
	float * data = new float[width * height * depth * numColorComponents];

	texture.bind();
	glGetTexImage(texture.type, 0, texture.format(),
			texture.dataType(), data);
	texture.unbind();


	delete [] data;
	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void TerrainDemo::inspectVboData(const TerrainBlock &block) {

	const GLsizei numBytes = block.getBytesPerVertexBuffer();
	float * data = new float[numBytes/sizeof(float)];

	glBindBuffer(GL_ARRAY_BUFFER, block.getPositionVertexBuffer());
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, numBytes, data);


	delete [] data;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECK_GL_ERRORS;
}
