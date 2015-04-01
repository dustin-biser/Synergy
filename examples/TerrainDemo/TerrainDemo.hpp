/// TerrainDemo

#pragma once

#include <Synergy/Synergy.hpp>

#include "GlfwOpenGlWindow.hpp"
#include "TerrainBlockGenerator.hpp"
#include "PostProcessRenderer.hpp"


//-- Forward Declarations:
class TerrainBlockGenerator;
class MarchingCubesSurfacePolygonizer;
class RockDensityGenerator;
class LightingOven;
class TerrainRenderer;
class Skybox;
class RenderTarget;
class PostProcessRenderer;


const int kScreenWidth = 1024;
const int kScreenHeight = 768;

// Terrain Block density texture dimenions:
const int kGridWidth = 32;
const int kGridHeight = 32;
const int kGridDepth = 32;


class TerrainDemo : public GlfwOpenGlWindow {

public:
    ~TerrainDemo();

    static std::shared_ptr<GlfwOpenGlWindow> getInstance();

private:
    TerrainDemo() = default; // Singleton. Prevent direct construction.

	TerrainBlockGenerator * terrainBlockGenerator;
    MarchingCubesSurfacePolygonizer * surfacePolygonizer;
	RockDensityGenerator * rockDensityGenerator;
	LightingOven * lightingOven;
	TerrainRenderer * terrainRenderer;
	Skybox * skybox;
	RenderTarget * renderTarget;
	PostProcessRenderer * postProcessRenderer;

	std::map<WorldSpaceIndex, TerrainBlock *> blockMap;

	bool renderSkybox = true;
	bool gammaCorrection = false;

    virtual void init();
    virtual void logic();
    virtual void draw();
    virtual void cleanup();
	virtual void keyInput(int key, int action, int mods);

	void toggleTerrainNormalVisualization();
	void toggleVoxelEdgeVisualization();
    void setupCamera();
	void setupGl();
	void initSkyboxTextures();


	// TODO -- Dustin Remove after testing:
	void inspectTextureData(const Synergy::Texture3D & texture );
};
