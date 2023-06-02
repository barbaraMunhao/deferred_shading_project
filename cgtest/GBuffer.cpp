
// OVERVIEW: GBuffer.cpp
// ========
// Source file for OpenGL FBO.
//
// Author: Bianca Namie
// Last revision: 27/06/2019

#include "GBuffer.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLFramebuffer implementation
// =============
	GBuffer::~GBuffer()
	{
		glDeleteTextures(2, _textures);
		glDeleteFramebuffers(1, &_fbo);
	}

	GBuffer::GBuffer(uint32_t width, uint32_t height, int levels) :
		_W{ width }, _H{ height }
	{
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

		glGenTextures(3, _textures);
	//	glBindTexture(GL_TEXTURE_2D, _textures[0]);
		glBindTexture(GL_TEXTURE_2D, _textures[0]);
		glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA32UI, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//	glBindTexture(GL_TEXTURE_2D, _textures[1]);
		glBindTexture(GL_TEXTURE_2D, _textures[1]);
		glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA32UI, width, height);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//glBindTexture(GL_TEXTURE_2D, _textures[2]);
		glBindTexture(GL_TEXTURE_2D, _textures[2]);
		glTexStorage2D(GL_TEXTURE_2D, levels, GL_DEPTH_COMPONENT32F, width, height);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textures[0], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, _textures[1], 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _textures[2], 0);

		const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

		glDrawBuffers(2, drawBuffers);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void
		GBuffer::use()
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
		GBuffer::disuse()
	{
		if (_inUse)
		{
			glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDrawBuffer(GL_BACK);
			_inUse = false;
		}
	}
} // end namespace cg
