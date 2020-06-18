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
	void initShapeMap();

	struct shape_map {
		uint16_t m_Value;
		inline uint8_t x_pos() { return _BITFIELD(m_Value, 0, 4); }
		inline uint8_t y_pos() { return _BITFIELD(m_Value, 4, 4); }
		inline uint8_t sheet() { return _BITFIELD(m_Value, 8, 2); }
	};

	OpenGLContext m_ctx;

	gl::shader shader;
	gl::texture atlas;
	std::unordered_map<uint16_t, shape_map> m_ShapeMaps;
};

class glrenderer_exception final : public std::exception {
	char m_Msg[512];
public:
	glrenderer_exception(const char* msg);
	inline const char* what() const override { return m_Msg; }
};