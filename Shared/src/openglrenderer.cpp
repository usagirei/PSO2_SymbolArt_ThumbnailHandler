#include "openglrenderer.h"
#include "stb_image.h"
#include "resource.h"

#ifdef _USRDLL
extern HINSTANCE g_hInstDll;
#endif

HINSTANCE OpenGLContext::hInstance;
HWND OpenGLContext::hWnd;
HGLRC OpenGLContext::hGlrc;
HDC OpenGLContext::hDc;

bool OpenGLContext::m_initialized;
int OpenGLContext::m_instanceCount;

std::mutex OpenGLContext::mutex;

bool m_ShouldClose;

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			PostQuitMessage(0);
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		m_ShouldClose = true;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;       // message handled
}

inline char* getRcdata(HINSTANCE hInstance, int resId, int& size) {
	HRSRC hVs = FindResource(hInstance, MAKEINTRESOURCE(resId), RT_RCDATA);
	HGLOBAL hVss = LoadResource(hInstance, hVs);
	size = SizeofResource(hInstance, hVs);
	return static_cast<char*>(LockResource(hVss));
}

OpenGLRenderer::OpenGLRenderer()
{
	int vsl, fsl, texl;
	char* vss = getRcdata(m_ctx.hInstance, IDR_SHADER_DEFAULT_VS, vsl);
	char* fss = getRcdata(m_ctx.hInstance, IDR_SHADER_DEFAULT_FS, fsl);
	char* atl = getRcdata(m_ctx.hInstance, IDR_TEXTURE_ATLAS, texl);

	int atl_w, atl_h;
	uint8_t* atl_data = stbi_load_from_memory((uint8_t*)atl, texl, &atl_w, &atl_h, nullptr, 4);

	{
		std::lock_guard<std::mutex> lock{ m_ctx.mutex };

		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		gl::shader s(vss, fss, vsl, fsl);
		shader = std::move(s);

		atlas.image2d(atl_w, atl_h, GL_RGBA, GL_UNSIGNED_BYTE, atl_data);
	}

	stbi_image_free(atl_data);
}

OpenGLRenderer::~OpenGLRenderer()
{

}

void OpenGLRenderer::Render(SarFile& sar, int texW, int texH, void* bgraData)
{
	// Prepare Data

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec4> colors;
	std::vector<glm::vec2> uvs;
	std::vector<GLuint> indices;

	glm::vec2 uv[] = { {0,0},{0,1},{1,0}, {1,1} };
	uint32_t iv[] = { 1,0,3,3,0,2 };
	int ic = 0;
	for (int i = 0; i < sar.numLayers(); i++) {
		int lid = sar.numLayers() - 1 - i;
		SarLayer& l = sar.layer(lid);

		if (l.hidden())
			continue;

		// 0 2
		// 1 3
		for (int j = 0; j < 4; j++) {
			glm::vec3 pos{ l.vertex(j).x, l.vertex(j).y, (i + 1) / 512.f };

			vertices.push_back(pos);
			glm::vec4 col(l.red(), l.green(), l.blue(), l.alpha());
			colors.push_back(col / 255.f);

			int shape = l.shape();
			int dx = shape % 32;
			int dy = shape / 32;

			glm::vec2 baseUv = uv[j];
			baseUv = baseUv + glm::vec2(dx, dy);
			baseUv = baseUv / 32.f;
			uvs.push_back(baseUv);
		}
		for (int j = 0; j < 6; j++) {
			indices.push_back(iv[j] + (4 * ic));
		}
		ic++;
	}

	glm::mat4 view = glm::mat4(1.0f);
	view = glm::scale(view, glm::vec3(1, 1, 1));
	view = glm::translate(view, glm::vec3(-128.f, -128.f, 0.f));

	float halfW = sar.width() / 2.f;
	float halfH = sar.height() / 2.f;
	glm::mat4 proj = glm::ortho(-halfW - 0.5f, halfW - 0.5f, -halfH - 0.5f, halfH - 0.5f);


	// OpenGL Drawing
	{
		std::lock_guard<std::mutex> lock{ m_ctx.mutex };

		gl::framebuffer fbo(texW, texH);
		gl::framebuffer msaa_fbo(texW, texH);

		shader.setUniform("view", view);
		shader.setUniform("proj", proj);

		glActiveTexture(GL_TEXTURE0);
		atlas.bind(GL_TEXTURE_2D);
		shader.setUniform("tex", 0);

		gl::arraybuffer vbo;
		vbo.bufferData(&vertices[0], vertices.size(), GL_STATIC_DRAW);

		gl::arraybuffer attr1;
		attr1.bufferData(&colors[0], colors.size(), GL_STATIC_DRAW);

		gl::arraybuffer attr2;
		attr2.bufferData(&uvs[0], uvs.size(), GL_STATIC_DRAW);

		gl::elementbuffer ebo;
		ebo.bufferData(&indices[0], indices.size(), GL_STATIC_DRAW);

		gl::vertexarray vao;

		vao.bind();
		ebo.bind();

		vbo.bind();
		glVertexAttribPointer(0, 3, GL_FLOAT, FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		attr1.bind();
		glVertexAttribPointer(1, 4, GL_FLOAT, FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);

		attr2.bind();
		glVertexAttribPointer(2, 2, GL_FLOAT, FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);

		vao.unbind();

		// Draw
		msaa_fbo.bind();

		glViewport(0, 0, texW, texH);

		glClearColor(1, 1, 1, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		shader.use();
		vao.bind();

		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		msaa_fbo.unbind();

		msaa_fbo.blit(fbo);
		fbo.bind();

		glReadPixels(0, 0, texW, texH, GL_BGRA, GL_UNSIGNED_BYTE, bgraData);

		fbo.unbind();
	}
}

OpenGLContext::OpenGLContext()
{
#ifdef _USRDLL
	hInstance = g_hInstDll;
#else
	hInstance = GetModuleHandle(NULL);
#endif

	std::lock_guard<std::mutex> lock{ mutex };
	m_instanceCount++;
	if (m_initialized)
		return;

	m_initialized = true;

	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WindowProcedure;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = L"PSO2Thumb";

	ATOM wndCls = RegisterClassEx(&wcex);

	HWND fakeWND = CreateWindow(
		L"PSO2Thumb", L"PSO2 Thumbnail",      // window class, title
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // style
		0, 0,                       // position x, y
		1, 1,                       // width, height
		NULL, NULL,                 // parent window, menu
		hInstance, NULL);           // instance, param

	HDC fakeDC = GetDC(fakeWND);        // Device Context

	PIXELFORMATDESCRIPTOR fakePFD;
	ZeroMemory(&fakePFD, sizeof(fakePFD));
	fakePFD.nSize = sizeof(fakePFD);
	fakePFD.nVersion = 1;
	fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	fakePFD.iPixelType = PFD_TYPE_RGBA;
	fakePFD.cColorBits = 32;
	fakePFD.cAlphaBits = 8;
	fakePFD.cDepthBits = 24;

	int fakePFDID = ChoosePixelFormat(fakeDC, &fakePFD);
	if (fakePFDID == 0) {
		throw glrenderer_exception("Failed to Initialize DC");
	}

	if (SetPixelFormat(fakeDC, fakePFDID, &fakePFD) == false) {
		throw glrenderer_exception("Failed to Initialize DC");
	}

	HGLRC fakeRC = wglCreateContext(fakeDC);    // Rendering Contex
	if (fakeRC == 0) {
		throw glrenderer_exception("Failed to Create OpenGL Context");
	}

	if (wglMakeCurrent(fakeDC, fakeRC) == false) {
		throw glrenderer_exception("Failed to Create OpenGL Context");
	}

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
	wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
	if (wglChoosePixelFormatARB == nullptr) {
		throw glrenderer_exception("Failed to Create OpenGL Context");
	}

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
	wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
	if (wglCreateContextAttribsARB == nullptr) {
		throw glrenderer_exception("Failed to Create OpenGL Context");
	}

	hWnd = CreateWindow(
		L"PSO2Thumb", L"PSO2 Thumbnail",        // class name, window name
		WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // style
		0, 0,       // posx, posy
		192, 192,    // width, height
		NULL, NULL,                     // parent window, menu
		hInstance, NULL);               // instance, param

	hDc = GetDC(hWnd);

	const int pixelAttribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 4,
		0
	};

	int pixelFormatID;
	UINT numFormats;
	bool status = wglChoosePixelFormatARB(hDc, pixelAttribs, NULL, 1, &pixelFormatID, &numFormats);

	if (status == false || numFormats == 0) {
		throw glrenderer_exception("Failed to Create OpenGL Context");
	}

	gladLoadGL();

	PIXELFORMATDESCRIPTOR PFD;
	DescribePixelFormat(hDc, pixelFormatID, sizeof(PFD), &PFD);
	SetPixelFormat(hDc, pixelFormatID, &PFD);

	int  contextAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, GLVersion.major,
		WGL_CONTEXT_MINOR_VERSION_ARB, GLVersion.minor,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	hGlrc = wglCreateContextAttribsARB(hDc, 0, contextAttribs);
	if (hGlrc == NULL) {
		throw glrenderer_exception("Failed to Create OpenGL Context");
	}

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(fakeRC);
	ReleaseDC(fakeWND, fakeDC);
	DestroyWindow(fakeWND);

	if (!wglMakeCurrent(hDc, hGlrc)) {
		throw glrenderer_exception("Failed to Create OpenGL Context");
	}
}

OpenGLContext::~OpenGLContext()
{
	std::lock_guard<std::mutex> lock{ mutex };
	m_instanceCount--;
	if (m_instanceCount != 0)
		return;

	wglDeleteContext(hGlrc);
	ReleaseDC(hWnd, hDc);
	DestroyWindow(hWnd);
	m_initialized = false;
}

glrenderer_exception::glrenderer_exception(const char* msg)
{
	strcpy_s(m_Msg, msg);
}
