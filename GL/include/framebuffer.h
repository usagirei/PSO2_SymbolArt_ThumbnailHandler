#pragma once

#include "gl.h"
#include <exception>

namespace gl {

	class framebuffer {
	public:
		GLuint ID;
		GLuint TexID;
		GLuint ColorRBO;
		GLuint DepthRBO;

		int width;
		int height;

		framebuffer(int w, int h);
		framebuffer(int w, int h, int samples);

		framebuffer(framebuffer&) = delete;
		framebuffer& operator=(framebuffer&) = delete;

		framebuffer(framebuffer&&);
		framebuffer& operator=(framebuffer&&);

		~framebuffer();

		void bind();
		void unbind();

		void blit(framebuffer& target);
	};

	class framebuffer_exception : public std::exception {
		char m_Msg[512];
	public:
		framebuffer_exception(const char* msg);
		inline const char* message() { return m_Msg; }
	};
}
