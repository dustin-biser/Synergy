set (SOURCE_FILES
Core/Exception.hpp  
Core/Math.hpp  
Core/ObjFileLoader.cpp  
Core/ObjFileLoader.hpp  
Core/Resources.cpp  
Core/Resources.hpp  
Core/Settings.hpp

Graphics/Camera.cpp  
Graphics/Frustum.cpp  
Graphics/GlErrorCheck.cpp  
Graphics/PostProcessor.cpp  
Graphics/RenderTarget.cpp  
Graphics/ShaderException.hpp  
Graphics/ShaderProgram.hpp  
Graphics/Texture.inl
Graphics/Camera.hpp  
Graphics/Frustum.hpp  
Graphics/GlErrorCheck.hpp  
Graphics/PostProcessor.hpp  
Graphics/RenderTarget.hpp  
Graphics/ShaderProgram.cpp    
Graphics/Texture.hpp
)

add_library(Synergy STATIC ${SOURCE_FILES})

set (DIRECTORIES
source/Synergy
source/Synergy/Core
source/Synergy/Graphics
)

MESSAGE(STATUS "TESTTTT" ${DIRECTORIES})

target_include_directories (Synergy PUBLIC ${DIRECTORIES})
