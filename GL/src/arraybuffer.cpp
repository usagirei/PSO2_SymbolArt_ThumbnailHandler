#include "arraybuffer.h"

using namespace gl;

vertexarray::vertexarray()
{
	glGenVertexArrays(1, &ID);
}

vertexarray::vertexarray(vertexarray&& o)
{
	ID = o.ID;
	o.ID = 0;
}

vertexarray& gl::vertexarray::operator=(vertexarray&& o)
{
	if (ID)	glDeleteVertexArrays(1, &ID);
	ID = o.ID;
	o.ID = 0;
	return *this;
}

vertexarray::~vertexarray()
{
	if (ID) {
		glDeleteVertexArrays(1, &ID);
		ID = 0;
	}
}

void vertexarray::bind()
{
	glBindVertexArray(ID);
}

void vertexarray::unbind()
{
	glBindVertexArray(0);
}


buffer::buffer(GLenum type) : Type(type)
{
	glGenBuffers(1, &ID);
}

buffer::buffer(buffer&& o)
{
	ID = o.ID;
	o.ID = 0;
}

buffer& buffer::operator=(buffer&& o)
{
	if (ID) glDeleteBuffers(1, &ID);
	ID = o.ID;
	o.ID = 0;
	return *this;
}

buffer::~buffer()
{
	if (ID) {
		glDeleteBuffers(1, &ID);
		ID = 0;
	}
}

void buffer::bind()
{
	glBindBuffer(Type, ID);
}

void buffer::unbind()
{
	glBindBuffer(Type, 0);
}
