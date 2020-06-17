#pragma once

#include <exception>
#include <mutex>

#include "gl.h"
#include "renderer.h"

struct OpenGLContext {

	OpenGLContext();
	~OpenGLContext();

	static std::mutex mutex;

	static bool m_initialized;
	static int m_instanceCount;

	static HINSTANCE hInstance;
	static HWND hWnd;
	static HGLRC hGlrc;
	static HDC hDc;
};

class OpenGLRenderer final : public SarRenderer {
public:
	OpenGLRenderer();
	~OpenGLRenderer();

	void Render(SarFile& sar, int texW, int texH, void* bgraData) override;

private:
	OpenGLContext m_ctx;

	gl::shader shader;
	gl::texture atlas;
};

class glrenderer_exception : public std::exception {
	char m_Msg[512];
public:
	glrenderer_exception(const char* msg);
	inline const char* what() { return m_Msg; }
};