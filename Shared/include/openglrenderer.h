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
	static constexpr int FLAG_HD = 1;

	OpenGLRenderer();
	~OpenGLRenderer();

	void Render(SarFile& sar, gl::framebuffer& fbo);

	void SetFlag(int flag, bool value) override;
	void Render(SarFile& sar, int texW, int texH, void* bgraData) override;

private:
	void initShapeAttrs();

	struct shape_attr {
		uint16_t m_Value;
		bool m_EnableSdf;

		inline uint8_t x_pos() { return _BITFIELD(m_Value, 0, 4); }
		inline uint8_t y_pos() { return _BITFIELD(m_Value, 4, 4); }
		inline uint8_t sheet() { return _BITFIELD(m_Value, 8, 2); }
		inline bool    sdf() { return m_EnableSdf; }
	};

	OpenGLContext m_ctx;

	gl::shader shader;
	gl::texture atlas;
	std::unordered_map<uint16_t, shape_attr> m_ShapeAttrs;
};

class glrenderer_exception final : public std::exception {
	char m_Msg[512];
public:
	glrenderer_exception(const char* msg);
	inline const char* what() const override { return m_Msg; }
};