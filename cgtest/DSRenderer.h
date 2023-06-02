//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2019 Orthrus Group.                               |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: GLMeshRenderer.h
// ========
// Class definition for OpenGL mesh renderer.
//
// Author: Paulo Pagliosa
// Last revision: 28/05/2019

#ifndef __DSRenderer_h
#define __DSRenderer_h

#include "graphics/Application.h"
#include "graphics/GLGraphics3.h"
#include "graphics/Light.h"
#include "Object.h"
#include<vector>

namespace cg
{ // begin namespace cg
/////////////////////////////////////////////////////////////////////
//
// GLMeshRenderer: OpenGL mesh renderer class
// ==============
class DSRenderer: public SharedObject
{
public:
	
  constexpr static auto maxLights = 8;

  enum RenderMode
  {
    Wireframe = 1,
    HiddenLines = 2,
    Flat = 4,
    Smooth = 0
  };

  enum RenderBits
  {
    UseLights = 1,
    UserVertexColors = 2
  };

  using RenderFlags = Flags<RenderBits>;

  RenderMode renderMode{Smooth};
  RenderFlags flags{RenderBits::UseLights};

  // Constructor.
	DSRenderer(Camera* camera=nullptr);

  Camera* camera() const
  {
    return _camera;
  }

  auto useVertexColors() const
  {
    return flags.isSet(RenderBits::UserVertexColors);
  }

  void setCamera(Camera* camera);

  void begin();

 
	void setTexture(GLuint texture);

  void render(Reference<TriangleMesh>, Material *, const mat4f&, const mat3f&);
  //void render(TriangleMesh&, const vec3f&, const mat3f&, const vec3f&);
	void render(Reference<cg::Object> obj) {
		auto n = mat3f{ obj->transform().worldToLocalMatrix().transposed() };
		render(obj->mesh(), obj->material(), obj->transform().localToWorldMatrix(), n);
	}
  void end();

private:
  struct GLState
  {
    GLSL::Program* program;
    bool depthTest;   // <--
    int texture;
    int vao;
  };

  struct LightPropertyLoc
  {
    GLint position;
    GLint color;
  };

  Reference<Camera> _camera;
  GLSL::Program _program;
  mat4f _viewportMatrix;
  int _lightCount{};
  GLuint _texture{};
  GLint _mvMatrixLoc;
  GLint _normalMatrixLoc;
  GLint _mvpMatrixLoc;
  GLint _viewportMatrixLoc; 
  GLint _useTextureLoc; 
	GLint _nsLoc;
	GLint _OsLoc;
	GLint _OdLoc;
  GLState _lastState;

  void initProgram();
  void initUniformLocations();
 

  void updateView();	
}; // DSRenderer



inline void
DSRenderer::setTexture(GLuint texture)
{
	_texture = texture;
}




} // end namespace cg






#endif // __DSRenderer_h
