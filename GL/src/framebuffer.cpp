#include "framebuffer.h"

using namespace gl;

framebuffer::framebuffer(int w, int h)
	: width(w), height(h)
{
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D, TexID);
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &ColorRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, ColorRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, w, h);

	glGenRenderbuffers(1, &DepthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);

	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, TexID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ColorRBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw framebuffer_exception("Incomplete Framebuffer");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


framebuffer::framebuffer(int w, int h, int samples)
	: width(w), height(h)
{
	glGenTextures(1, &TexID);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, TexID);
	{
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, w, h, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	glGenRenderbuffers(1, &ColorRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, ColorRBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_RGBA8, w, h);

	glGenRenderbuffers(1, &DepthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, DepthRBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, w, h);

	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, TexID, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, ColorRBO);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw framebuffer_exception("Incomplete Framebuffer");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

framebuffer::framebuffer(framebuffer&& o)
{
	ID = o.ID;
	TexID = o.TexID;
	ColorRBO = o.ColorRBO;
	DepthRBO = o.DepthRBO;
	width = o.width;
	height = o.height;

	o.ID = 0;
	o.ColorRBO = 0;
	o.DepthRBO = 0;
	o.width = 0;
	o.height = 0;
}

framebuffer& framebuffer::operator=(framebuffer&& o)
{
	if (ID) {
		glDeleteFramebuffers(1, &ID);
		glDeleteTextures(1, &TexID);
		glDeleteRenderbuffers(1, &ColorRBO);
		glDeleteRenderbuffers(1, &DepthRBO);
	}

	ID = o.ID;
	TexID = o.TexID;
	ColorRBO = o.ColorRBO;
	DepthRBO = o.DepthRBO;
	width = o.width;
	height = o.height;

	o.ID = 0;
	o.ColorRBO = 0;
	o.DepthRBO = 0;
	o.width = 0;
	o.height = 0;

	return *this;
}

framebuffer::~framebuffer()
{
	if (ID) {
		glDeleteFramebuffers(1, &ID);
		glDeleteTextures(1, &TexID);
		glDeleteRenderbuffers(1, &ColorRBO);
		glDeleteRenderbuffers(1, &DepthRBO);

		ID = 0;
		ColorRBO = 0;
		DepthRBO = 0;
		width = 0;
		height = 0;
	}
}

void framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void framebuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void framebuffer::blit(framebuffer& target)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.ID);
	glDrawBuffer(GL_BACK);
	glBlitFramebuffer(0, 0, width, height, 0, 0, target.width, target.height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

framebuffer_exception::framebuffer_exception(const char* msg)
{
	strcpy_s(m_Msg, msg);
}

