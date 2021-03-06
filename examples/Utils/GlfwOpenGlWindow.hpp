/**
 * @brief Abstract class for other classes to inherit from that want access to an
 * GLFW window and OpenGL context for rendering.
 */

#pragma once


// Force GLFW to include gl3.h core functionality instead of gl.h deprecated code.
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

#include <Synergy/Graphics/Camera.hpp>

#include <glm/glm.hpp>

#include <string>
#include <memory>
#include <chrono>

#include "CameraController.hpp"

class GlfwOpenGlWindow {
public:
    virtual ~GlfwOpenGlWindow();

    static std::shared_ptr<GlfwOpenGlWindow> getInstance();

    void create(int width,
                int height,
                const std::string & windowTitle,
                double secondsPerFrame = 1/60.0f);

protected:

    Synergy::Camera camera;
    CameraController cameraController;

    static std::shared_ptr<GlfwOpenGlWindow> p_instance;

    // TODO Dustin - Make some of these methods private since derived classes wont need
    // to use them.

    GlfwOpenGlWindow();

    static void error_callback(int error, const char* description);

    static void keyInputCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
    void keyInputBase(int key, int action, int mods);

    static void windowResizeCallBack(GLFWwindow * window, int width, int height);

    static void mouseScrollCallBack(GLFWwindow * window, double xOffSet, double yOffSet);
    void mouseScrollBase(double xOffSet, double yOffSet);

    static void mouseButtonCallBack(GLFWwindow * window, int button , int actions, int mods);
    void mouseButtonInputBase(int button , int actions, int mods);

    static void cursorPositionCallBack(GLFWwindow * window, double xPos, double yPos);
    void cursorPositionBase(double xPos, double yPos);

    static void cursorEnterCallBack(GLFWwindow * window, int entered);
    void cursorEnterBase(int entered);

    void registerGlfwCallBacks();

    void destroyPrevWindowCheck();

    void centerWindow();

    void initNewOpenGlContext();

    void switchToFullScreen();

    void switchToWindowedMode();

    void setupCamera();

    int defaultFramebufferWidth() const;

    int defaultFramebufferHeight() const;

    // Virtual methods.
    virtual void init() { }
    virtual void setupGl();
    virtual void reloadShaderProgram() { }
    virtual void logic() { }
    virtual void draw() { }
    virtual void cleanup() { }

    // Virtual Callback methods.
    virtual void cursorEnter(int entered);
    virtual void cursorPosition(double xPos, double yPos);
    virtual void mouseButtonInput(int button , int actions, int mods);
    virtual void mouseScroll(double xOffSet, double yOffSet);
    virtual void resize(int width, int height);
    virtual void keyInput(int key, int action, int mods);


private:
    GLFWwindow * window;
    GLFWwindow * prevWindow;
    GLFWmonitor * monitor;
    std::string windowTitle;
    int windowWidth;
    int windowHeight;
    int framebufferPixelWidth;
    int framebufferPixelHeight;

    bool paused;
    bool fullScreen;
    bool destroyPrevWindow;

    std::chrono::duration<double> frameLimiter(
            double desiredSecondsPerFrame,
            const std::chrono::steady_clock::time_point & startTime) const;
};
