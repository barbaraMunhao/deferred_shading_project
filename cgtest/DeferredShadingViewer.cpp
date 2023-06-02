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
// OVERVIEW: DeferredShadingViewer.cpp
// ========
// Source file for DeferredShading file viewer.
//
// Author: Bárbara Munhão, Bianca Namie e Wellington Castro
// Last revision: 05/07/2019

#include "graphics/GLImage.h"
#include "DeferredShadingViewer.h"
#include "Utils.h"

using namespace cg;


/////////////////////////////////////////////////////////////////////
//
// DeferredShadingViewer implementation
// =========


DeferredShadingViewer::DeferredShadingViewer(uint32_t width, uint32_t height):
  _width{width},
  _height{height},
  GLWindow{"Def. Shading Viewer", width, height},
  _program{"DS final drawer"}
{
	_materials = new Material[_numMaterials];
	_mesh = GLGraphics3::sphere();
}

void
DeferredShadingViewer::initialize()
{
  constexpr float d = 10;

	Application::loadShaders(_program, "shaders/ds.vs", "shaders/ds.fs");
	initProgram();
	Assets::initialize();  
	buildMaterials();
	createObjects();
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  _gb = new GBuffer{_width, _height};           
	
  _camera = new Camera;                        
  _camera->setPosition({0, 0, d});
  _camera->setDistance(d);
  _camera->setAspectRatio((float)width() / height());
 	_gRenderer = new DSRenderer(_camera);    
	_gRenderer->setTexture(_texture);

	initializeLights();
	glEnable(GL_LINE_SMOOTH);
  glDisable(GL_DEPTH_TEST);
}

void 
DeferredShadingViewer::buildMaterials()
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

	vec4f ambient{ frand(0, 1), frand(0, 1), frand(0, 1), frand(0, 1)};
	for (; i < _numMaterials; i++)
	{
		_materials[i].ambient = ambient; // am
		_materials[i].diffuse = vec4f{ frand(0, 1), frand(0, 1), frand(0, 1), frand(0, 1), }; // di
		_materials[i].spot = vec4f{ frand(0, 1), frand(0, 1), frand(0, 1), frand(0, 1), }; // Os
		_materials[i].shine = (int)frand(30, 60); // spec
		_materials[i].texture = nullptr;
	}
}

void 
DeferredShadingViewer::createObjects()
{
	float maxRadius{ _edgeSize / (2.f *_numObjects) };
	float minRadius{ maxRadius / 2.f };

	float{ _edgeSize / 2.f };
	vec3f corner{-_edgeSize / 2.f, -_edgeSize / 2.f , -_edgeSize / 2.f };
	
	for (int y = 0; y < _numObjects; y++)
	{
		float py{ 2 * y * maxRadius + maxRadius};
		for (int x = 0; x < _numObjects; x++)
		{
			float px{2 * x * maxRadius + maxRadius};
			for (int z = 0; z < _numObjects; z++)
			{
				vec3f position{ px, py, 2*z*maxRadius + maxRadius};
				auto obj = new Object{ _mesh, _materials + (rand() % _numMaterials) };
				auto& t = obj->transform();
				t.setPosition(corner + position);
				t.setLocalScale(frand(minRadius, maxRadius));
				_objects.push_back(obj);
			}
		}
	}			
}

void
DeferredShadingViewer::terminate()
{
  glDeleteVertexArrays(1, &_vao);
  Assets::clear();
  deleteTextures(_defaultTextures);
	delete [] _materials;
	_objects.~vector();
}

void DeferredShadingViewer::update()
{	
	if (_stepsCounter % _steps == 0)
	{
		_dt = _dtAcc / _steps;
		_dt = deltaTime();
		_dtAcc = deltaTime();
	}
	else 
	{
		_dtAcc += deltaTime();
	}
}

void DeferredShadingViewer::gui()
{
	ImGui::Begin("Avg deltatime");
	char buffer[20];
	snprintf(buffer, 20, "%f",_dt);
	ImGui::InputText("", buffer, 20, ImGuiInputTextFlags_ReadOnly);
	ImGui::End();
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
DeferredShadingViewer::render()
{		
	_gb->use();
	clear();
	_gRenderer->begin();	
	cleanBuffers();
	for (int i = 0; i < _objects.size(); i++)
	{	
			_gRenderer->render(_objects[i]); // execute program 1 
	}  
	_gRenderer->end();  
	_gb->disuse();

	_program.use();
	updateUniforms();
	auto textures = _gb->colors();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *(textures));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *(textures+1));
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // execute program 2

	_stepsCounter++;
}

void
DeferredShadingViewer::cleanBuffers()
{
	static const GLuint uint_zeros[] = { 0, 0, 0, 0 };
	static const GLfloat float_zeros[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	static const GLfloat float_ones[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glDrawBuffers(2, draw_buffers);
	glClearBufferuiv(GL_COLOR, 0, uint_zeros);
	glClearBufferuiv(GL_COLOR, 1, uint_zeros);
	glClearBufferfv(GL_DEPTH, 0, float_ones);
}

void 
DeferredShadingViewer::updateUniforms()
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
			vec4f{ _lights[i].color.r, _lights[i].color.g, _lights[i].color.b, 1.0f});

		_program.setUniform(_lightLocs[i].type, (int) _lights[i].flags);
	}
	_program.setUniform(_lightCountLoc, _numLights);
}

bool
DeferredShadingViewer::windowResizeEvent(int width, int height)
{
  _camera->setAspectRatio((float)width / height);
  return true;
}

bool
DeferredShadingViewer::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _camera->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
DeferredShadingViewer::mouseButtonInputEvent(int button, int actions, int mods)
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
DeferredShadingViewer::mouseMoveEvent(double xPos, double yPos)
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

inline void
DeferredShadingViewer::initProgram()
{
	auto cp = GLSL::Program::current();
	_program.use();
	initUniformLocations();	
	GLSL::Program::setCurrent(cp);
}

inline void 
DeferredShadingViewer::initializeLights()
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

	_lights[4].position = vec3f{ -2, -3, 0 };
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
DeferredShadingViewer::initUniformLocations()
{	
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