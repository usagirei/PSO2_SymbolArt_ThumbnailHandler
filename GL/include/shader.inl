#include "shader.h"

namespace gl {

	template<>
	inline void shader::_setUniform<glm::vec2>(const char* name, int count, const glm::vec2* value)
	{
		use();
		glUniform2fv(getUniformLocation(name), count, glm::value_ptr(*value));
	}

	template<>
	inline void shader::_setUniform<glm::vec3>(const char* name, int count, const glm::vec3* value)
	{
		use();
		glUniform2fv(getUniformLocation(name), count, glm::value_ptr(*value));
	}

	template<>
	inline void shader::_setUniform<glm::vec4>(const char* name, int count, const glm::vec4* value)
	{
		use();
		glUniform2fv(getUniformLocation(name), count, glm::value_ptr(*value));
	}

	template<>
	inline void shader::_setUniform<glm::mat4>(const char* name, int count, const glm::mat4* value)
	{
		use();
		auto ptr = glm::value_ptr(*value);
		glUniformMatrix4fv(getUniformLocation(name), count, GL_FALSE, ptr);
	}

	template<>
	inline void shader::_setUniform<GLfloat>(const char* name, int count, const GLfloat* value)
	{
		use();
		glUniform1fv(getUniformLocation(name), count, value);
	}

	template<>
	inline void shader::_setUniform<GLint>(const char* name, int count, const GLint* value)
	{
		use();
		glUniform1iv(getUniformLocation(name), count, value);
	}
}