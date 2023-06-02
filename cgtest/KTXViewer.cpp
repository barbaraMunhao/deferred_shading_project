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
// OVERVIEW: KTXViewer.cpp
// ========
// Source file for KTX file viewer.
//
// Author: Paulo Pagliosa
// Last revision: 01/06/2019

#include "graphics/GLImage.h"
#include "KTXViewer.h"

using namespace cg;


/////////////////////////////////////////////////////////////////////
//
// KTXViewer implementation
// =========
static const char* vertexShader =
  "#version 430 core\n"
  "void main() {\n"
  "const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.0, 1.0),\n"
  "vec4( 1.0, -1.0, 0.0, 1.0),\n"
  "vec4(-1.0,  1.0, 0.0, 1.0),\n"
  "vec4( 1.0,  1.0, 0.0, 1.0));\n"
  "gl_Position = vertices[gl_VertexID];\n"
  "}";

static const char* fragmentShader =
  "#version 430 core\n"
  "uniform sampler2D s;\n"
  "out vec4 color;\n"
  "void main() {\n"
  "color = texture(s, gl_FragCoord.xy / textureSize(s, 0));\n"
  "}";

inline uint32_t
checkerboard(const Color& color = Color::white, const int cellSize = 16)
{
  const auto boardSize = cellSize * 16;
  ImageBuffer buffer{boardSize, boardSize};
  const Pixel pattern[2] = {Color::black, color};

  for (int p = 0, iy = 0, y = 0; y < 16; ++y, iy ^= 1)
    for (int i = 0; i < cellSize; ++i)
      for (int ix = iy, x = 0; x < 16; ++x, ix ^= 1)
        for (int j = 0; j < cellSize; ++j)
          buffer[p++] = pattern[ix];

  GLuint texture;

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, boardSize, boardSize);
  glTexSubImage2D(GL_TEXTURE_2D,
    0,
    0,
    0,
    boardSize,
    boardSize,
    GL_RGB,
    GL_UNSIGNED_BYTE,
    buffer.data());
  return texture;
}

KTXViewer::KTXViewer(int width, int height):
  GLWindow{"KTX Viewer", width, height},
  _program{"Background Drawer"}
{
  _material.spot = Color::gray;
  _material.shine = 20;
}

inline void
KTXViewer::buildDefaultTextures()
{
  _defaultTextures["None"] = 0;
  _defaultTextures["Checkerboard"] = checkerboard(Color::yellow, 32);
}

void
KTXViewer::initialize()
{
  constexpr float d = 3;

  _program.setShaders(vertexShader, fragmentShader).use();
  Assets::initialize();
  buildDefaultTextures();
  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  _fb = new GLFramebuffer{512, 512};
  _camera = new Camera;
  _camera->setPosition({0, 0, d});
  _camera->setDistance(d);
  //_camera->setAspectRatio((float)width() / height());
  _meshRenderer = new GLMeshRenderer{_camera};
  if (true)
  {
    Light lights[2];

    lights[0].position = {+d, +d, 0};
    lights[0].flags.set(Light::LightBits::Camera);
    lights[1].position = {-d, -d, 0};
    lights[1].flags.set(Light::LightBits::Camera);
    _meshRenderer->begin();
    _meshRenderer->setLights(lights, lights + 2);
    _meshRenderer->end();
  }
  _mesh = GLGraphics3::sphere();
  glEnable(GL_LINE_SMOOTH);
  glDisable(GL_DEPTH_TEST);
}

void
KTXViewer::terminate()
{
  glDeleteVertexArrays(1, &_vao);
  Assets::clear();
  deleteTextures(_defaultTextures);
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
KTXViewer::render()
{
  _fb->use();
  clear(Color::darkGray);
  _meshRenderer->begin();
  _meshRenderer->setMaterial(_material);
  _meshRenderer->render(*_mesh);
  _meshRenderer->end();
  _fb->disuse();
  clear();
  if (_texture != 0)
    if (_background)
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    else
    {
      auto w = std::min(512, width());
      auto h = std::min(512, height());
      auto x = (width() - w) >> 1;
      auto y = (height() - h) >> 1;

      GLImage::draw(_texture, x, y, w, h);
    }
}

bool
KTXViewer::windowResizeEvent(int width, int height)
{
  //_camera->setAspectRatio((float)width / height);
  return true;
}

bool
KTXViewer::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _camera->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
KTXViewer::mouseButtonInputEvent(int button, int actions, int mods)
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
KTXViewer::mouseMoveEvent(double xPos, double yPos)
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
KTXViewer::setTexture(const std::string& texName, uint32_t texture)
{
  _texName = texName;
  glBindTexture(GL_TEXTURE_2D, texture);
  _material.texture = (void*)(intptr_t)(_texture = texture);
}

inline void
KTXViewer::setTexture(cg::TextureMapIterator tit)
{
  setTexture(tit->first, Assets::loadTexture(tit));
}

inline void
KTXViewer::inspectTexture()
{
  constexpr int maxSize = 32;
  char buffer[maxSize];

  snprintf(buffer, maxSize, "%s", _texName.c_str());
  ImGui::InputText("Texture", buffer, maxSize, ImGuiInputTextFlags_ReadOnly);
  if (ImGui::BeginDragDropTarget())
  {
    if (auto * payload = ImGui::AcceptDragDropPayload("TextureIt"))
      setTexture(*(TextureMapIterator*)payload->Data);
    ImGui::EndDragDropTarget();
  }
  ImGui::SameLine();
  if (ImGui::Button("...###TextureIt"))
    ImGui::OpenPopup("TextureItPopup");
  if (ImGui::BeginPopup("TextureItPopup"))
  {
    for (auto p : _defaultTextures)
      if (ImGui::Selectable(p.first.c_str()))
        setTexture(p.first, p.second);

    auto& textures = Assets::textures();

    if (!textures.empty())
    {
      ImGui::Separator();
      for (auto tit = textures.begin(); tit != textures.end(); ++tit)
        if (ImGui::Selectable(tit->first.c_str()))
          setTexture(tit);
    }
    ImGui::EndPopup();
  }
  if (_texture != 0)
  {
    ImGui::Checkbox("Background", &_background);
    ImGui::SameLine();
  }

  static auto preview = true;

  ImGui::Checkbox("Preview", &preview);
  if (preview)
  {
    ImGui::SameLine();
    if (ImGui::Button("Model"))
      ImGui::OpenPopup("ModelPopup");
    if (ImGui::BeginPopup("ModelPopup"))
    {
      if (ImGui::MenuItem("Quad"))
        _mesh = GLGraphics3::quad();
      if (ImGui::MenuItem("Sphere"))
        _mesh = GLGraphics3::sphere();
      ImGui::EndPopup();
    }

    constexpr int x = 16;
    auto s = ImGui::GetWindowWidth() - x;
    auto image = _fb->colors();

    ImGui::SetCursorPosX(x >> 1);
    ImGui::Image((void*)(intptr_t)image, {s, s}, {0, 1}, {1, 0});
  }
}

inline void
KTXViewer::assetsWindow()
{
  ImGui::Begin("Assets");
  if (ImGui::CollapsingHeader("Textures"))
  {
    auto& textures = Assets::textures();

    for (auto tit = textures.begin(); tit != textures.end(); ++tit)
    {
      auto texName = tit->first.c_str();
      auto selected = false;

      ImGui::Selectable(texName, &selected);
      if (ImGui::BeginDragDropSource())
      {
        ImGui::Text(texName);
        ImGui::SetDragDropPayload("TextureIt", &tit, sizeof(tit));
        ImGui::EndDragDropSource();
      }
    }
  }
  ImGui::End();
}

inline void
KTXViewer::inspectorWindow()
{
  ImGui::Begin("Inspector");
  inspectTexture();
  ImGui::End();
}

void
KTXViewer::gui()
{
  inspectorWindow();
  assetsWindow();
}
