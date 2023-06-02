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
// OVERVIEW: DeferredShadingViewer.h
// ========
// Class definition for Deferred Shanding file viewer.
//
// Author: Bárbara Munhão, Bianca Namie e Wellington Castro.
// Last revision: 05/07/2019

#ifndef __DSViewer_h
#define __DSViewer_h

#include "graphics/GLMeshRenderer.h"
#include "graphics/GLWindow.h"
#include "Assets.h"
#include "GBuffer.h"
#include "DSRenderer.h"
#include "Object.h"
#include <vector>


/////////////////////////////////////////////////////////////////////
//
// DeferredShadingViewer: Deferred Shading file viewer class
// =========
class DeferredShadingViewer final: public cg::GLWindow
{
public:

  DeferredShadingViewer(uint32_t width, uint32_t height);

private:
	uint32_t _width, _height;
	int _n;	
	int _stepsCounter{};
	float _dt{};
	float _dtAcc{};
	static constexpr int _steps{60};
	static constexpr int _numMaterials{ 20 };
	static constexpr int _numObjects{ 10 };
	static constexpr float _edgeSize{10};

	cg::Reference<cg::DSRenderer> _gRenderer;
  cg::Reference<cg::GBuffer> _gb;
  cg::Reference<cg::Camera> _camera;
  cg::TextureMap _defaultTextures;
  cg::GLSL::Program _program;

	std::vector<cg::Reference<cg::Object>> _objects;
	cg::Material* _materials;
	cg::Reference<cg::TriangleMesh> _mesh; 
	cg::Light _lights[10]; 

  std::string _texName;
  GLuint _texture{};
  GLuint _vao{};
  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;
  bool _dragging{false};
  bool _background{false};

  void initialize() override;
  void render() override;
  void terminate() override;
	void update() override;
	void gui() override;

  bool windowResizeEvent(int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

 
 
	void buildMaterials();
	void initializeLights(); 
	void createObjects();
	void cleanBuffers();

	void updateUniforms();

	struct LightPropertyLoc
	{
		GLint position;
		GLint color;
		GLint type;
	};	

	int _numLights{ 0 };
	
	GLint _lightCountLoc;
	LightPropertyLoc _lightLocs[10];
	void initUniformLocations();
	void initProgram();
}; //DeferredShadingViewer

#endif // __DSViewer_h
