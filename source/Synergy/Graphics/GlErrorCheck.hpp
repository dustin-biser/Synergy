/**
 * GlErrorCheck
 */

#pragma once

//-- OpenGL:
#ifdef _APPLE_
	#include <OpenGL/gl3.h>
#elif defined __linux
	#define GL_GLEXT_PROTOTYPES
  	#include <GL/gl.h>
	#include <GL/glcorearb.h>
#endif

#include <string>

#if(DEBUG)
#define CHECK_GL_ERRORS Synergy::checkGLErrors(__FILE__, __LINE__)
#define CHECK_FRAMEBUFFER_COMPLETENESS Synergy::checkFramebufferCompleteness()
#else
#define CHECK_GL_ERRORS
#define CHECK_FRAMEBUFFER_COMPLETENESS
#endif

namespace Synergy {

	void checkGLErrors(const std::string & currentFileName, int currentLineNumber);

    void checkFramebufferCompleteness();

}
