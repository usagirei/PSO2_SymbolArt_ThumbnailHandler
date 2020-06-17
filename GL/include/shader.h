#pragma once

#include "gl.h"
#include <exception>
#include <unordered_map>

#include "glm/mat4x4.hpp"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace gl {
	class shader {
	public:
		GLuint ID;

		shader();
		~shader();
		 
		shader(const char* vertex, const char* fragment);
		shader(const char* vertex, const char* fragment, int vertex_size, int frag_size);

		shader(shader&) = delete;
		shader& operator=(shader&) = delete;

		shader(shader&&);
		shader& operator=(shader&&);

		void use();

		template<typename V>
		void setUniform(const char* name, V value);

	private:
		uint64_t hashUniformName(const char* name, int len = 0);
		uint32_t getUniformLocation(const char* name);
		
		std::unordered_map<uint64_t, uint32_t> m_Uniforms;
	};

	class shader_exception : public std::exception {
		char m_Msg[512];
	public:
		shader_exception(const char* msg);
		inline const char* what() { return m_Msg; }
	};
}

#include "shader.inl"