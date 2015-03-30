/**
* @brief MarchineCubesExample
*
*/

#pragma once

#include <Synergy/Synergy.hpp>
using namespace Synergy;

#include "GlfwOpenGlWindow.hpp"

// Forward Declaration:
class MarchingCubesRenderer;


const int kScreenWidth = 1024;
const int kScreenHeight = 768;

// Density grid dimensions
const int kGridWidth = 3;
const int kGridHeight = 3;
const int kGridDepth = 3;

const float kIsoSurfaceValue = 0.0f;


class TerrainDemo : public GlfwOpenGlWindow {

public:
    ~TerrainDemo();

    static std::shared_ptr<GlfwOpenGlWindow> getInstance();

private:
    TerrainDemo() = default; // Singleton. Prevent direct construction.

    MarchingCubesRenderer * marchingCubesRenderer;

    Synergy::Texture3D densityGrid;

    virtual void init();
    virtual void logic();
    virtual void draw();
    virtual void cleanup();

    void createTextureStorage();
    void fillVolumeDensityTexture();
    void setupCamera();
};
