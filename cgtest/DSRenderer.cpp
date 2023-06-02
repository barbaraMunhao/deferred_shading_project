
// OVERVIEW: DSRenderer.cpp
// ========
// Source file for renderer.
//
// Author: 
// Last revision: 

#include "DSRenderer.h"

namespace cg
{ // begin namespace cg


inline void
DSRenderer::initUniformLocations()
{
  _mvMatrixLoc = _program.uniformLocation("mvMatrix");
  _normalMatrixLoc = _program.uniformLocation("normalMatrix");
  _mvpMatrixLoc = _program.uniformLocation("mvpMatrix");
	_OsLoc = _program.uniformLocation("Os");
	_nsLoc = _program.uniformLocation("ns");
	_OdLoc = _program.uniformLocation("Od");
	_useTextureLoc = _program.uniformLocation("use_texture");
 // _viewportMatrixLoc = _program.uniformLocation("viewportMatrix");
}

inline void
DSRenderer::initProgram()
{
  auto cp = GLSL::Program::current();

  _program.use();
  initUniformLocations();
  
  GLSL::Program::setCurrent(cp);
}

DSRenderer::DSRenderer(Camera* camera):
  _camera{camera == nullptr ? new Camera{} : camera},
	_program{ "Mesh Renderer" }
{
	Application::loadShaders(_program, "shaders/g.vs", "shaders/g.fs");	
  initProgram();
}

inline void
DSRenderer::updateView()
{
  _camera->update();

  GLint v[4];
  
  glGetIntegerv(GL_VIEWPORT, v);

  float w2 = v[2] / 2.0f;
  float h2 = v[3] / 2.0f;

  _viewportMatrix[0].set(w2, 0, 0, 0);
  _viewportMatrix[1].set(0, h2, 0, 0);
  _viewportMatrix[2].set(0, 0, 1, 0);
  _viewportMatrix[3].set(v[0] + w2, v[1] + h2, 0, 0);
}

void
DSRenderer::setCamera(Camera* camera)
{
  if (camera != _camera)
    (_camera = nullptr != camera ? camera : new Camera{})->update();
}

void
DSRenderer::begin()
{
  _lastState.program = GLSL::Program::current();
  _lastState.depthTest = glIsEnabled(GL_DEPTH_TEST);
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &_lastState.vao);
  glGetIntegerv(GL_TEXTURE_BINDING_2D, &_lastState.texture);
  updateView();
  glPolygonMode(GL_FRONT_AND_BACK, (renderMode != Wireframe) + GL_LINE);
  glEnable(GL_DEPTH_TEST);
  _program.use();
 // _program.setUniformMat4(_viewportMatrixLoc, _viewportMatrix);
  //if (_lightCount == 0)
    //setDefaultLights();
}

void
DSRenderer::end()
{
  GLSL::Program::setCurrent(_lastState.program);
  if (!_lastState.depthTest)
    glDisable(GL_DEPTH_TEST);
  glBindVertexArray(_lastState.vao);
  glBindTexture(GL_TEXTURE_2D, _lastState.texture);
}


/*void
DSRenderer::setMaterial(const Material* material)
{
 /* _program.setUniformVec4(_OaLoc, material.ambient);
  _program.setUniformVec4(_OdLoc, material.diffuse);
  _program.setUniformVec4(_OsLoc, material.spot);
  _program.setUniform(_nsLoc, material.shine);
  if (material.texture == nullptr)
    _texture = 0;
  else
    _texture = (GLuint)(intptr_t)material.texture; // TODO
}*/

inline auto
normalMatrix(const cg::mat3f& n, const cg::Camera* c)
{
  return cg::mat3f{c->worldToCameraMatrix()} * n;
}


void
DSRenderer::render(Reference<TriangleMesh> mesh, Material* material, const mat4f& t, const mat3f& n)
{
	
  auto mv = _camera->worldToCameraMatrix() * t;

  _program.setUniformMat4(_mvMatrixLoc, mv);
	_program.setUniformMat3(_normalMatrixLoc, normalMatrix(n, _camera));
	_program.setUniformMat4(_mvpMatrixLoc, _camera->projectionMatrix() * mv);	

	
	_program.setUniformVec4(_OsLoc, material->spot);
	_program.setUniform(_nsLoc, material->shine);

    if (material->texture!= nullptr)
		{			
			glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, (GLuint)material->texture);
			_program.setUniform(_useTextureLoc, 1);
		}
		else {
			_program.setUniformVec4(_OdLoc, material->diffuse);
			_program.setUniform(_useTextureLoc, 0);
		}

  auto m = glMesh(mesh.get());
  m->bind();	
  glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);
}

} // end namespace cg
