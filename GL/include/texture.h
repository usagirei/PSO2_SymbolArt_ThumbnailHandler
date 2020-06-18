#pragma once

#include "gl.h"
#include <exception>

namespace gl {

	class texture {
	public:
		GLuint ID;
		GLenum BoundTo;

		texture();
		texture(GLint internalFormat);

		texture(texture&) = delete;
		texture& operator=(texture&) = delete;

		texture(texture&&) noexcept;
		texture& operator=(texture&&) noexcept;

		~texture();
		void image2d(int w, int h, GLenum pixelFormat, GLenum pixelType, void* pixelData);

		void bind(GLenum target);
		void unbind();
	private:
		GLint internalFormat;
	};
}
