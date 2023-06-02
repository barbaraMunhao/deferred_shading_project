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
// OVERVIEW: SViewer.cpp
// ========
// Source file for S file viewer.
//
// Author: Bárbara Munhão e Wellington Castro.
// Last revision: 04/07/2019

#include "Utils.h"
#include "graphics/GLImage.h"
#include "SViewer.h"

using namespace cg;


/////////////////////////////////////////////////////////////////////
//
// SViewer implementation
// =========



SViewer::SViewer(int width, int height):
  GLWindow{"S Viewer", width, height},
  _program{"Background Drawer"}
{
	_materials = new Material[_numMaterials];
	_mesh = GLGraphics3::sphere();
}

void
SViewer::initialize()
{
	constexpr float d = 10;

	Application::loadShaders(_program, "shaders/s.vs", "shaders/s.fs");
	initProgram();
	Assets::initialize();
	buildMaterials();
	createObjects();
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);		
	_camera = new Camera;
	_camera->setPosition({ 0, 0, d });
	_camera->setDistance(d);
	_camera->setAspectRatio((float)width() / height());	
	initializeLights();
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);
}

void
SViewer::terminate()
{
  glDeleteVertexArrays(1, &_vao);
  Assets::clear();
  deleteTextures(_defaultTextures);	
	delete [] _materials;
	_objects.~vector();
}

void SViewer::update()
{
	if (_stepsCounter % _steps == 0)
	{
		_dt = _dtAcc / _steps;
		_dtAcc = deltaTime();
	}
	else
	{
		_dtAcc += deltaTime();
	}
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
SViewer::render()
{
	clear(Color::black);
	updateUniforms();
	for (int i = 0; i < _objects.size(); i++)
	{
		auto n = mat3f{ _objects[i]->transform().worldToLocalMatrix().transposed() };

		renderMesh(_objects[i]->mesh(), _objects[i]->material(), 
			_objects[i]->transform().localToWorldMatrix(), n);
	}
}

inline auto
normalMatrix(const cg::mat3f& n, const cg::Camera* c)
{
	return cg::mat3f{ c->worldToCameraMatrix() } *n;
}

void
SViewer::renderMesh(cg::Reference<cg::TriangleMesh> mesh, cg::Material* material, const cg::mat4f& t, const cg::mat3f& n)
{
	auto mv = _camera->worldToCameraMatrix() * t;

	_program.setUniformMat4(_mvMatrixLoc, mv);
	_program.setUniformMat3(_normalMatrixLoc, normalMatrix(n, _camera));
	_program.setUniformMat4(_mvpMatrixLoc, _camera->projectionMatrix() * mv);


	_program.setUniformVec4(_OsLoc, material->spot);
	_program.setUniform(_nsLoc, material->shine);

	if (material->texture != nullptr)
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

bool
SViewer::windowResizeEvent(int width, int height)
{
  _camera->setAspectRatio((float)width / height);
  return true;
}

bool
SViewer::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _camera->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
SViewer::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  (void)mods;
  if (button == GLFW_MOUSE_BUTTON_LEFT)
    _dragging = actions == GLFW_PRESS;
  if (_dragging)
    cursorPosition(_pivotX, _pivotY);
  return true;
}

bool
SViewer::mouseMoveEvent(double xPos, double yPos)
{
  if (!_dragging)
    return false;
  _mouseX = (int)xPos;
  _mouseY = (int)yPos;

  const auto dx = (_pivotX - _mouseX);
  const auto dy = (_pivotY - _mouseY);

  _pivotX = _mouseX;
  _pivotY = _mouseY;
  if (dx != 0 || dy != 0)
  {
    const auto da = -_camera->viewAngle() * CAMERA_RES;
    _camera->rotateYX(dx * da, dy * da, true);
  }
  return true;
}

void
SViewer::gui()
{
	ImGui::Begin("Avg deltatime");
	char buffer[20];
	snprintf(buffer, 20, "%f", _dt);
	ImGui::InputText("", buffer, 20, ImGuiInputTextFlags_ReadOnly);
	ImGui::End();
}

void 
SViewer::updateUniforms()
{
	for (auto i = 0; i < _numLights; i++)
	{
		if (_lights[i].flags == Light::Directional)
		{
			_program.setUniformVec4(_lightLocs[i].position,
				vec4f{ _camera->worldToCameraMatrix().transformVector(_lights[i].position), 1 });
		}
		else
		{
			_program.setUniformVec4(_lightLocs[i].position,
				vec4f{ _camera->worldToCamera(_lights[i].position), 1.0 });
		}

		_program.setUniformVec4(_lightLocs[i].color,
			vec4f{ _lights[i].color.r, _lights[i].color.g, _lights[i].color.b, 1.0f });

		_program.setUniform(_lightLocs[i].type, (int)_lights[i].flags);
	}
	_program.setUniform(_lightCountLoc, _numLights);
}


void 
SViewer::initProgram()
{	
	_program.use();
	initUniformLocations();
}

void 
SViewer::buildMaterials()
{
	int i = 0;

	for (auto it = Assets::textures().begin(); it != Assets::textures().end(); it++)
	{
		if (i == _numMaterials)
			return;

		_materials[i].spot = vec4f{ frand(0, 1), frand(0, 1), frand(0, 1), frand(0, 1), }; // Os
		_materials[i].shine = (int)frand(30, 60); // ns
		_materials[i].texture = (void*)Assets::loadTexture(it);
		i++;
	}

	vec4f ambient{ frand(0, 1), frand(0, 1), frand(0, 1), frand(0, 1) };
	for (; i < _numMaterials; i++)
	{
		_materials[i].ambient = ambient; // am
		_materials[i].diffuse = vec4f{ frand(0, 1), frand(0, 1), frand(0, 1), frand(0, 1), }; // di
		_materials[i].spot = vec4f{ frand(0, 1), frand(0, 1), frand(0, 1), frand(0, 1), }; // Os
		_materials[i].shine = (int)frand(30, 60); // spec
		_materials[i].texture = nullptr;
	}
}


void SViewer::createObjects()
{
	float maxRadius{ _edgeSize / (2.f * _numObjects) };
	float minRadius{ maxRadius / 2.f };

	float{ _edgeSize / 2.f };
	vec3f corner{ -_edgeSize / 2.f, -_edgeSize / 2.f , -_edgeSize / 2.f };

	for (int y = 0; y < _numObjects; y++)
	{
		float py{ 2 * y * maxRadius + maxRadius };
		for (int x = 0; x < _numObjects; x++)
		{
			float px{ 2 * x * maxRadius + maxRadius };
			for (int z = 0; z < _numObjects; z++)
			{
				vec3f position{ px, py, 2 * z * maxRadius + maxRadius };
				auto obj = new Object{ _mesh, _materials + (rand() % _numMaterials) };
				auto& t = obj->transform();
				t.setPosition(corner + position);
				t.setLocalScale(frand(minRadius, maxRadius));
				_objects.push_back(obj);
			}
		}
	}
}

void SViewer::initializeLights()
{	
	_lights[0].position = vec3f{ 0, -1, 0 };
	_lights[0].color = Color::cyan;
	_lights[0].flags = Light::Directional;
	
	_lights[1].position = vec3f{ 0, 3, 3 };
	_lights[1].color = Color::white;

	_lights[2].position = vec3f{ -1, 0, 0 };
	_lights[2].color = Color::yellow;
	_lights[2].flags = Light::Directional;

	_lights[3].position = vec3f{ 0, 1, 0 };
	_lights[3].color.r = 1.0;
	_lights[3].color.g = 0.4;
	_lights[3].color.b = 0.4;

	_lights[4].position = vec3f{ -2, -3, 0};
	_lights[4].color.r = 0.6;
	_lights[4].color.g = 0.8;
	_lights[4].color.b = 1.0;

	_lights[5].position = vec3f{ 1, 0, 0 };
	_lights[5].color.r = 1.0;
	_lights[5].color.g = 0.6;
	_lights[5].color.b = 1.0;
	_lights[5].flags = Light::Directional;

	_lights[6].position = vec3f{ 0, 1, 0 };
	_lights[6].color.r = 0.8;
	_lights[6].color.g = 0.0;
	_lights[6].color.b = 0.2;
	_lights[6].flags = Light::Directional;

	_lights[7].position = vec3f{ 5, 2, 6 };
	_lights[7].color.r = 0.0;
	_lights[7].color.g = 0.8;
	_lights[7].color.b = 0.8;


	_lights[8].position = vec3f{ 2, 2, 2 };
	_lights[8].color.r = 0.5;
	_lights[8].color.g = 0.5;
	_lights[8].color.b = 0.5;

	_lights[9].position = vec3f{ 0, 0, 1 };
	_lights[9].color.r = 0.6;
	_lights[9].color.g = 1.0;
	_lights[9].color.b = 0.2;
	_lights[9].flags = Light::Directional;

	_numLights = 10;
}

inline void
SViewer::initUniformLocations()
{
	_mvMatrixLoc = _program.uniformLocation("mvMatrix");
	_normalMatrixLoc = _program.uniformLocation("normalMatrix");
	_mvpMatrixLoc = _program.uniformLocation("mvpMatrix");
	_OsLoc = _program.uniformLocation("Os");
	_nsLoc = _program.uniformLocation("ns");
	_OdLoc = _program.uniformLocation("Od");
	_useTextureLoc = _program.uniformLocation("use_texture");

	_lightCountLoc = _program.uniformLocation("num_lights");

	_lightLocs[0].position = _program.uniformLocation("lights[0].position");
	_lightLocs[0].color = _program.uniformLocation("lights[0].color");
	_lightLocs[0].type = _program.uniformLocation("lights[0].type");

	_lightLocs[1].position = _program.uniformLocation("lights[1].position");
	_lightLocs[1].color = _program.uniformLocation("lights[1].color");
	_lightLocs[1].type = _program.uniformLocation("lights[1].type");

	_lightLocs[2].position = _program.uniformLocation("lights[2].position");
	_lightLocs[2].color = _program.uniformLocation("lights[2].color");
	_lightLocs[2].type = _program.uniformLocation("lights[2].type");

	_lightLocs[3].position = _program.uniformLocation("lights[3].position");
	_lightLocs[3].color = _program.uniformLocation("lights[3].color");
	_lightLocs[3].type = _program.uniformLocation("lights[3].type");

	_lightLocs[4].position = _program.uniformLocation("lights[4].position");
	_lightLocs[4].color = _program.uniformLocation("lights[4].color");
	_lightLocs[4].type = _program.uniformLocation("lights[4].type");

	_lightLocs[5].position = _program.uniformLocation("lights[5].position");
	_lightLocs[5].color = _program.uniformLocation("lights[5].color");
	_lightLocs[5].type = _program.uniformLocation("lights[5].type");

	_lightLocs[6].position = _program.uniformLocation("lights[6].position");
	_lightLocs[6].color = _program.uniformLocation("lights[6].color");
	_lightLocs[6].type = _program.uniformLocation("lights[6].type");

	_lightLocs[7].position = _program.uniformLocation("lights[7].position");
	_lightLocs[7].color = _program.uniformLocation("lights[7].color");
	_lightLocs[7].type = _program.uniformLocation("lights[7].type");

	_lightLocs[8].position = _program.uniformLocation("lights[8].position");
	_lightLocs[8].color = _program.uniformLocation("lights[8].color");
	_lightLocs[8].type = _program.uniformLocation("lights[8].type");

	_lightLocs[9].position = _program.uniformLocation("lights[9].position");
	_lightLocs[9].color = _program.uniformLocation("lights[9].color");
	_lightLocs[9].type = _program.uniformLocation("lights[9].type");	
}