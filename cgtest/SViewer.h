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
// OVERVIEW: SViewer.h
// ========
// Class definition for S file viewer.
//
// Author: Bárbara Munhão e Wellington Castro.
// Last revision: 04/07/2019

#ifndef __SViewer_h
#define __SViewer_h

#include "graphics/Application.h"
#include "graphics/GLMeshRenderer.h"
#include "graphics/GLWindow.h"
#include "Assets.h"
#include "GLFramebuffer.h"
#include "Object.h"
#include <vector>

/////////////////////////////////////////////////////////////////////
//
// SViewer: S file viewer class
// =========
class SViewer final: public cg::GLWindow
{
public:
  SViewer(int width, int height);

private:
	int _stepsCounter{};
	float _dt{};
	float _dtAcc{};
	static constexpr int _steps{ 60 };
	static constexpr int _numMaterials{ 20 };
	static constexpr int _numObjects{ 10 };
	static constexpr float _edgeSize{ 10};

  cg::Reference<cg::GLMeshRenderer> _meshRenderer;
  cg::Reference<cg::TriangleMesh> _mesh;
	std::vector<cg::Reference<cg::Object>> _objects;
	cg::Material* _materials;
	cg::Light _lights[10]; 

  cg::Reference<cg::Camera> _camera;
  cg::TextureMap _defaultTextures;
  cg::GLSL::Program _program;
  cg::Material _material;
  std::string _texName;
  GLuint _texture{};
  GLuint _vao{};


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
	GLint _mvMatrixLoc;
	GLint _normalMatrixLoc;
	GLint _mvpMatrixLoc;
	GLint _viewportMatrixLoc;
	GLint _useTextureLoc;
	GLint _nsLoc;
	GLint _OsLoc;
	GLint _OdLoc;

	void initUniformLocations();	
	void initProgram();
	void buildMaterials();
	void initializeLights(); 
	void createObjects();	

  int _pivotX;
  int _pivotY;
  int _mouseX;
  int _mouseY;
  bool _dragging{false};
  bool _background{false};

  void initialize() override;
  void render() override;
  void gui() override;
  void terminate() override;
	void update() override;

  bool windowResizeEvent(int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

	void renderMesh(cg::Reference<cg::TriangleMesh> mesh,
		cg::Material* material, const cg::mat4f& t, const cg::mat3f& n);
 
}; // SViewer

#endif // __SViewer_h
