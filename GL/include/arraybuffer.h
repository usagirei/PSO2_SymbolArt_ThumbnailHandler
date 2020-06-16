#pragma once

#include "gl.h"

namespace gl {

	class vertexarray {
	public:
		GLuint ID;

		vertexarray();

		vertexarray(vertexarray&) = delete;
		vertexarray& operator=(vertexarray&) = delete;

		vertexarray(vertexarray&&);
		vertexarray& operator=(vertexarray&&);

		~vertexarray();

		void bind();
		void unbind();
	};

	class buffer {
	public:
		GLuint ID;
		GLenum Type;

		buffer(GLenum type);

		buffer(buffer&) = delete;
		buffer& operator=(buffer&) = delete;

		buffer(buffer&&);
		buffer& operator=(buffer&&);

		template<typename _Vertex>
		void bufferData(_Vertex* data, size_t num, GLenum mode);

		template<typename _Vertex>
		void bufferSubData(GLintptr offset, _Vertex* data, GLsizeiptr num);

		~buffer();

		void bind();
		void unbind();
	};

	class arraybuffer : public buffer {
	public:
		inline arraybuffer() : buffer(GL_ARRAY_BUFFER) {}
	};

	class elementbuffer : public buffer {
	public:
		inline elementbuffer() : buffer(GL_ELEMENT_ARRAY_BUFFER) {}
	};

	template<typename _Vertex>
	inline void buffer::bufferData(_Vertex* data, size_t num, GLenum mode)
	{
		bind();
		glBufferData(Type, sizeof(_Vertex) * num, data, mode);
		unbind();
	}

	template<>
	inline void buffer::bufferData(void* data, size_t num, GLenum mode) {
		bind();
		glBufferData(Type, num, data, mode);
		unbind();
	}

	template<typename _Vertex>
	inline void buffer::bufferSubData(GLintptr offset, _Vertex* data, GLsizeiptr num)
	{
		bind();
		glBufferSubData(Type, offset, sizeof(_Vertex) * num, data);
		unbind();
	}
}