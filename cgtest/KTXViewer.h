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
// OVERVIEW: KTXViewer.h
// ========
// Class definition for KTX file viewer.
//
// Author: Paulo Pagliosa
// Last revision: 04/06/2019

#ifndef __KTXViewer_h
#define __KTXViewer_h

#include "graphics/Application.h"
#include "graphics/GLMeshRenderer.h"
#include "graphics/GLWindow.h"
#include "Assets.h"
#include "GLFramebuffer.h"


/////////////////////////////////////////////////////////////////////
//
// KTXViewer: KTX file viewer class
// =========
class KTXViewer final: public cg::GLWindow
{
public:
  KTXViewer(int width, int height);

private:
  cg::Reference<cg::GLMeshRenderer> _meshRenderer;
  cg::Reference<cg::GLFramebuffer> _fb;
  cg::Reference<cg::TriangleMesh> _mesh;
  cg::Reference<cg::Camera> _camera;
  cg::TextureMap _defaultTextures;
  cg::GLSL::Program _program;
  cg::Material _material;
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
  void gui() override;
  void terminate() override;

  bool windowResizeEvent(int, int) override;
  bool scrollEvent(double, double) override;
  bool mouseButtonInputEvent(int, int, int) override;
  bool mouseMoveEvent(double, double) override;

  void inspectTexture();
  void inspectorWindow();
  void assetsWindow();

  void buildDefaultTextures();
  void setTexture(cg::TextureMapIterator);
  void setTexture(const std::string&, uint32_t);

}; // KTXViewer

#endif // __KTXViewer_h
