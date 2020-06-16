#include "shader.h"

namespace gl {

	template<>
	inline void shader::setUniform<glm::vec2>(const char* name, glm::vec2 value)
	{
		use();
		glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
	}

	template<>
	inline void shader::setUniform<glm::vec3>(const char* name, glm::vec3 value)
	{
		use();
		glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
	}

	template<>
	inline void shader::setUniform<glm::vec4>(const char* name, glm::vec4 value)
	{
		use();
		glUniform2fv(getUniformLocation(name), 1, glm::value_ptr(value));
	}

	template<>
	inline void shader::setUniform<glm::mat4>(const char* name, glm::mat4 value)
	{
		use();
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	template<>
	inline void shader::setUniform<GLfloat>(const char* name, GLfloat value)
	{
		use();
		glUniform1fv(getUniformLocation(name), 1, &value);
	}

	template<>
	inline void shader::setUniform<GLint>(const char* name, GLint value)
	{
		use();
		glUniform1iv(getUniformLocation(name), 1, &value);
	}

}