#include "texture.h"

using namespace gl;

texture::texture()
{
	glGenTextures(1, &ID);
}

texture::texture(texture&& o) noexcept
{
	ID = o.ID;
	o.ID = 0;
}

texture& texture::operator=(texture&& o) noexcept
{
	if (ID) glDeleteTextures(1, &ID);

	ID = o.ID;
	o.ID = 0;

	return *this;
}

texture::~texture()
{
	if (ID) {
		glDeleteTextures(1, &ID);
		ID = 0;
	}
}

void texture::bind(GLenum target) {
	glBindTexture(target, ID);
	BoundTo = target;
}

void texture::unbind() {
	glBindTexture(BoundTo, 0);
}

void texture::image2d(int w, int h, GLenum pixelFormat, GLenum pixelType, void* pixelData) {
	bind(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, pixelFormat, pixelType, pixelData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);
}
