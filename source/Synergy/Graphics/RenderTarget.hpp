/**
* RenderTarget
*/

#pragma once

#include <Synergy/Core/Settings.hpp>

#include <string>


namespace Synergy {

    /// Class for rendering the scene into a texture, instead of the display.
    class RenderTarget {
    public:
        RenderTarget();

        ~RenderTarget();

    private:

        GLuint framebuffer; 
        GLuint depthRBO;
         Synergy::Texture2D colorTexture;

    };

} // end namespace Synergy
