/// TerrainDemo

#pragma once

#include <Synergy/Synergy.hpp>

#include "GlfwOpenGlWindow.hpp"
#include "TerrainBlockGenerator.hpp"

// Forward Declarations:
class TerrainBlockGenerator;
class MarchingCubesSurfacePolygonizer;
class RockDensityGenerator;
class LightingOven;
class TerrainRenderer;
class Skybox;
class RenderTarget;


const int kScreenWidth = 1024;
const int kScreenHeight = 768;

// Terrain Block density texture dimenions:
const int kGridWidth = 4;
const int kGridHeight = 4;
const int kGridDepth = 4;


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

	std::map<WorldSpaceIndex, TerrainBlock *> blockMap;

	bool renderSkybox = true;

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
};
