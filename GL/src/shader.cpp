#include "shader.h"

namespace gl {
	shader::shader()
		: ID(0), m_Uniforms()
	{
	}

	shader::~shader()
	{
		if (ID) {
			glDeleteProgram(ID);
			ID = 0;
		}
	}


	shader::shader(const char* vertex, const char* fragment)
		: shader(vertex, fragment, 0, 0)
	{

	}

	shader::shader(const char* vertex, const char* fragment, int vertex_size, int frag_size)
		: ID(0)
	{
		GLint succ;
		GLuint vs, fs;
		char infoLog[512];

		vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &vertex, vertex_size ? &vertex_size : nullptr);
		glCompileShader(vs);

		glGetShaderiv(vs, GL_COMPILE_STATUS, &succ);
		if (!succ)
		{
			glGetShaderInfoLog(vs, 512, NULL, infoLog);
			throw shader_exception(infoLog);
		};

		fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fragment, frag_size ? &frag_size : nullptr);
		glCompileShader(fs);

		glGetShaderiv(fs, GL_COMPILE_STATUS, &succ);
		if (!succ)
		{
			glGetShaderInfoLog(fs, 512, NULL, infoLog);
			throw shader_exception(infoLog);
		};

		ID = glCreateProgram();
		glAttachShader(ID, vs);
		glAttachShader(ID, fs);
		glLinkProgram(ID);

		glGetProgramiv(ID, GL_LINK_STATUS, &succ);
		if (!succ)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			throw shader_exception(infoLog);
		}

		int numUniforms;
		char name[64];
		glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &numUniforms);
		for (int i = 0; i < numUniforms; i++) {
			int nameLen;
			glGetActiveUniformName(ID, i, 64, &nameLen, name);
			uint64_t uniformHash = hashUniformName(name, nameLen);
			m_Uniforms[uniformHash] = i;
		}

		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	shader::shader(shader&& o) :
		ID(0)
	{
		ID = o.ID;
		o.ID = 0;

		m_Uniforms = o.m_Uniforms;
	}

	shader& shader::operator=(shader&& o)
	{
		if (ID)	glDeleteProgram(ID);

		ID = o.ID;
		o.ID = 0;

		m_Uniforms = std::move(o.m_Uniforms);

		return *this;
	}

	void shader::use()
	{
		glUseProgram(ID);
	}

	uint64_t shader::hashUniformName(const char* name, int len)
	{
		static const uint64_t FNV_OFFSET_BASIS = 14695981039346656037ul;
		static const uint64_t FNV_PRIME = 1099511628211ul;

		if (len == 0)
			len = strnlen_s(name, 64);

		uint64_t hash = FNV_OFFSET_BASIS;
		for (int i = 0; i < len; i++, name++) {
			hash ^= *name;
			hash *= FNV_PRIME;
		}
		return hash;
	}

	uint32_t shader::getUniformLocation(const char* name)
	{
		uint64_t uHash = hashUniformName(name);
		auto id = m_Uniforms.find(uHash);
		if (id == m_Uniforms.end())
			throw shader_exception("Invalid Uniform Name");

		return id->second;
	}



	shader_exception::shader_exception(const char* msg) {
		strcpy_s(m_Msg, msg);
	}

}