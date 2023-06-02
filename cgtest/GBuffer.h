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
// OVERVIEW: GBuffer.h
// ========
// Class definition for OpenGL FBO.
//
// Author: Bianca Namie
// Last revision: 27/06/2019

#ifndef __GBuffer_h
#define __GBuffer_h

#include "graphics/GLBuffer.h"

namespace cg
{ // begin namespace cg


/////////////////////////////////////////////////////////////////////
//
// GLFramebuffer: OpenGL FBO class
// =============
	class GBuffer : public SharedObject
	{
	public:
		~GBuffer() override;

		GBuffer(uint32_t width, uint32_t height, int levels = 1);

		auto width() const
		{
			return _W;
		}

		auto height() const
		{
			return _H;
		}

		auto colors() const
		{
			return _textures;
		}

		void use();

		void disuse();

	private:
		uint32_t _W;
		uint32_t _H;
		bool _inUse{};
		GLuint _fbo;
		GLuint _textures[3];
		int _viewport[4];

	}; // GBuffer

} // end namespace cg

#endif // __GBuffer_h
