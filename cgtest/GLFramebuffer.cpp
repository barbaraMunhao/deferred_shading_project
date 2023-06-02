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
// OVERVIEW: GLFramebuffer.cpp
// ========
// Source file for OpenGL FBO.
//
// Author: Paulo Pagliosa
// Last revision: 04/06/2019

#include "GLFramebuffer.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLFramebuffer implementation
// =============
GLFramebuffer::~GLFramebuffer()
{
  glDeleteTextures(2, _textures);
  glDeleteFramebuffers(1, &_fbo);
}

GLFramebuffer::GLFramebuffer(uint32_t width, uint32_t height, int levels):
  _W{width}, _H{height}
{
  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glGenTextures(2, _textures);
  glBindTexture(GL_TEXTURE_2D, *_textures);
  glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGB8, width, height);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glBindTexture(GL_TEXTURE_2D, _textures[1]);
  glTexStorage2D(GL_TEXTURE_2D, levels, GL_DEPTH_COMPONENT32F, width, height);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *_textures, 0);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _textures[1], 0);

  const GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};

  glDrawBuffers(1, drawBuffers);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void
GLFramebuffer::use()
{
  if (!_inUse)
  {
    glGetIntegerv(GL_VIEWPORT, _viewport);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glViewport(0, 0, _W, _H);
    _inUse = true;
  }
}

void
GLFramebuffer::disuse()
{
  if (_inUse)
  {
    glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _inUse = false;
  }
}
} // end namespace cg
