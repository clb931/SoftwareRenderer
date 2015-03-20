#include "ATLAS_Stdafx.h"
#include "ATLAS_RenderContext.h"
#include "ATLAS_Model.h"
#include "ATLAS_Texture.h"
#include <Windows.h>
#undef CreateWindow

using namespace ATLAS;

LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ATLAS
{
	namespace Win32
	{
		struct Window
		{
			HINSTANCE		hinst;
			HWND			hwnd;
			HDC				hdc;
			BITMAPINFO		frame_buffer_info;
			void			*frame_buffer;
			uint32			fb_width;
			uint32			fb_height;
			uint32			fb_bpp;
			uint32			width;
			uint32			height;
			uint32			style;
		};

		bool32 HandleMessages()
		{
			MSG msg = {};
			while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT)
					return FALSE;

				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}

			return TRUE;
		}

		void ResizeScreenBuffers(Window *win32_window, int32 width, int32 height, int32 bpp = 32)
		{
			BITMAPINFO *fb_info = &win32_window->frame_buffer_info;

			if (win32_window->frame_buffer)
				VirtualFree(win32_window->frame_buffer, NULL, MEM_RELEASE);

			win32_window->fb_width = width;
			win32_window->fb_height = height;
			win32_window->fb_bpp = bpp;

			fb_info->bmiHeader.biSize = sizeof(fb_info->bmiHeader);
			fb_info->bmiHeader.biWidth = width;
			fb_info->bmiHeader.biHeight = height;
			fb_info->bmiHeader.biPlanes = 1;
			fb_info->bmiHeader.biBitCount = bpp;
			fb_info->bmiHeader.biCompression = BI_RGB;

			win32_window->frame_buffer = VirtualAlloc(0, width * height * bpp, MEM_COMMIT, PAGE_READWRITE);
		}
		void SwapBuffers(Window *win32_window)
		{
			BITMAPINFO *fb_info = &win32_window->frame_buffer_info;

			RECT rc = {};
			GetWindowRect(win32_window->hwnd, &rc);
			win32_window->width = rc.right - rc.left;
			win32_window->height = rc.bottom - rc.top;

			StretchDIBits(win32_window->hdc,
				0, 0, win32_window->width, win32_window->height,
				0, 0, win32_window->fb_width, win32_window->fb_height,
				win32_window->frame_buffer, fb_info, DIB_RGB_COLORS, SRCCOPY);
		};
		bool32 CreateWindow(Window *win32_window, LPCWSTR title, int32 window_width = CW_USEDEFAULT, int32 window_height = CW_USEDEFAULT, int32 style = AWS_VISIBLE, int32 buffer_width = 1280, int32 buffer_height = 720)
		{
			win32_window->hinst = GetModuleHandleW(NULL);

			LPCWSTR lpCLassName = title;

			WNDCLASSEXW wcex = {};
			wcex.cbSize = sizeof(WNDCLASSEXW);
			wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = WinProc;
			wcex.hInstance = win32_window->hinst;
			wcex.lpszClassName = lpCLassName;

			if (RegisterClassExW(&wcex)) {
				DWORD dwExStyle = 0;
				LPCWSTR lpWindowName = title;
				DWORD dwStyle = WS_OVERLAPPEDWINDOW | (style & AWS_VISIBLE ? WS_VISIBLE : NULL);
				INT nWindow_X = 2300;
				INT nWindow_Y = 100;
				INT nWindow_W = window_width;
				INT nWindow_H = window_height;

				win32_window->hwnd = CreateWindowExW(
					dwExStyle, lpCLassName,
					lpWindowName, dwStyle,
					nWindow_X, nWindow_Y,
					nWindow_W, nWindow_H,
					NULL, NULL,
					win32_window->hinst, NULL);

				if (!win32_window->hwnd)
				{
					win32_window = {};
					return FALSE;
				}

				win32_window->hdc = GetDC(win32_window->hwnd);

				RECT rc;
				GetWindowRect(win32_window->hwnd, &rc);

				win32_window->width = rc.right - rc.left;
				win32_window->height = rc.bottom - rc.top;
				win32_window->style = style;

				ResizeScreenBuffers(win32_window, buffer_width, buffer_height);

				return TRUE;
			}

			return FALSE;
		}
		void CloseWindow(Window *win32_window)
		{
			DestroyWindow(win32_window->hwnd);

			if (&win32_window->frame_buffer)
				VirtualFree(&win32_window->frame_buffer, NULL, MEM_RELEASE);

			win32_window = {};
		}
	}
}

GLOBAL Win32::Window win32_window = {};
GLOBAL bool32 CULL_FACES_ON = true;
GLOBAL bool32 DEPTH_TEST_ON = true;
GLOBAL AtlasEnum DRAW_STYLE = DRAW_TRIANGLES;
GLOBAL BlendMode BLEND_MODE = BLEND_NORMAL;

Texture font("font1.bmp");
Vertex verts[]
{
	Vertex(Vector4f(-0.5f, -0.5f, 0.5f), UV(0.0f, 0.0f), Color::YELLOW),// left  - bottom	- back
		Vertex(Vector4f(0.5f, -0.5f, 0.5f), UV(1.0f, 0.0f), Color::WHITE),	// right - bottom	- back
		Vertex(Vector4f(0.5f, -0.5f, -0.5f), UV(1.0f, 0.0f), Color::CYAN),	// left  - bottom	- front
		Vertex(Vector4f(-0.5f, -0.5f, -0.5f), UV(0.0f, 0.0f), Color::GREEN),// right - bottom	- front
		Vertex(Vector4f(-0.5f, 0.5f, 0.5f), UV(0.0f, 1.0f), Color::RED),	// left  - top		- back
		Vertex(Vector4f(0.5f, 0.5f, 0.5f), UV(1.0f, 1.0f), Color::MAGENTA),	// right - top		- back
		Vertex(Vector4f(0.5f, 0.5f, -0.5f), UV(1.0f, 1.0f), Color::BLUE), 	// left  - top		- front
		Vertex(Vector4f(-0.5f, 0.5f, -0.5f), UV(0.0f, 1.0f), Color::BLACK)	// right - top		- front
};
ATLAS::Polygon polys[]
{
	ATLAS::Polygon(0, 1, 4),	//front
		ATLAS::Polygon(4, 1, 5),	//front
		ATLAS::Polygon(1, 2, 5),	//bottom
		ATLAS::Polygon(2, 6, 5),	//bottom
		ATLAS::Polygon(2, 3, 6),	//back
		ATLAS::Polygon(3, 7, 6),	//back
		ATLAS::Polygon(3, 0, 7),	//top
		ATLAS::Polygon(0, 4, 7),	//top
		ATLAS::Polygon(4, 5, 7),	//right
		ATLAS::Polygon(5, 6, 7),	//right
		ATLAS::Polygon(3, 2, 0),	//left
		ATLAS::Polygon(2, 1, 0)	//left
};
Texture cubetexture("texture.bmp");
Model cube(verts, 8, polys, 12, &cubetexture);
Texture spaceshiptexture("spaceshiptexture.bmp");
Model spaceship("spaceship.3DS", &spaceshiptexture);
Model *model = &spaceship;
real32 z_position = -300.0f;

LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;
	switch (msg) {
	case WM_SIZE: {
		ATLAS::Win32::SwapBuffers(&win32_window);
	} break;

	case WM_KEYDOWN: {
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		if (wParam == VK_F1) {
			model = &spaceship;
			z_position = -300.0f;
		}
		if (wParam == VK_F2) {
			model = &cube;
			z_position = -2.0f;
		}
		if (wParam == VK_F3) {
			BLEND_MODE = (BlendMode)(BLEND_MODE + 1);
			if (BLEND_MODE > BLEND_LIGHTEN)
				BLEND_MODE = BLEND_NORMAL;
		}
		if (wParam == VK_F4)
			DRAW_STYLE = DRAW_TRIANGLES;
		if (wParam == VK_F5)
			DRAW_STYLE = DRAW_LINES;
		if (wParam == VK_F6)
			DRAW_STYLE = DRAW_POINTS;
		if (wParam == VK_F7)
			DEPTH_TEST_ON = !DEPTH_TEST_ON;
		if (wParam == VK_F8)
			CULL_FACES_ON = !CULL_FACES_ON;
	} break;

	case WM_CLOSE: {
		PostQuitMessage(0);
	} break;

	default: {
		result = DefWindowProcW(hwnd, msg, wParam, lParam);
	} break;
	}

	return result;
}

void DrawString(RenderContext *render_context, const char *str, Texture *font,
	real32 size = 24.0f, Color color = Color::WHITE, real32 x = -1.0f, real32 y = 1.0f)
{
	size /= 100.0f;
	const real32 one_over_16 = 1.0f / 16.0f;
	render_context->SetFlag(CULL_FACES, false);
	render_context->SetFlag(DEPTH_TEST, false);
	render_context->SetCurrentTexture(font);
	render_context->SetBlendMode(BLEND_DARKEN);

	uint32 length = strlen(str);
	Vertex vertices[4];
	uint32 k = 0;

	real32 half_width = (real32)win32_window.fb_width / 2.0f;
	real32 half_height = (real32)win32_window.fb_height / 2.0f;
	Matrix4f P = OrthograohicMatrix(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1000.0f);

	for (uint32 i = 0; i < length; ++i) {
		if (str[i] == '\n') {
			y -= size;
			k = 0;
			continue;
		}

		uint32 j = 0;
		int32 ch = str[i];
		real32 xPos = (real32)(ch % 16) * one_over_16;
		real32 yPos = (real32)(ch / 16) * one_over_16;


		// bottom - left
		vertices[j].pos.x = x + (size * 0.4f) * k;
		vertices[j].pos.y = y - size;
		vertices[j].uv.u = xPos;
		vertices[j].uv.v = 1.0f - yPos - one_over_16;
		vertices[j].color = color;
		j++;

		// bottom - right
		vertices[j].pos.x = x + (size * 0.4f) * (k + 1);
		vertices[j].pos.y = y - size;
		vertices[j].uv.u = xPos + one_over_16;
		vertices[j].uv.v = 1.0f - yPos - one_over_16;
		vertices[j].color = color;
		j++;

		// top - left
		vertices[j].pos.x = x + (size * 0.4f) * k;
		vertices[j].pos.y = y;
		vertices[j].uv.u = xPos;
		vertices[j].uv.v = 1.0f - yPos - 0.001f;
		vertices[j].color = color;
		j++;

		// top - right
		vertices[j].pos.x = x + (size * 0.4f) * (k + 1);
		vertices[j].pos.y = y;
		vertices[j].uv.u = xPos + one_over_16;
		vertices[j].uv.v = 1.0f - yPos - 0.001f;
		vertices[j].color = color;
		k++;

		vertices[0].pos *= P;
		vertices[1].pos *= P;
		vertices[2].pos *= P;
		vertices[3].pos *= P;

		render_context->DrawTriangle(vertices[0], vertices[1], vertices[2]);
		render_context->DrawTriangle(vertices[2], vertices[1], vertices[3]);
	}

	render_context->SetFlag(CULL_FACES, CULL_FACES_ON);
	render_context->SetFlag(DEPTH_TEST, DEPTH_TEST_ON);
	render_context->SetBlendMode(BLEND_MODE);
}
void DrawModel(RenderContext *render_context, Model *model, Matrix4f MVP)
{
	for (uint32 p = 0; p < model->m_NumPolygons; ++p) {
		Vertex v1 = model->m_Vertices[model->m_Polygons[p].v1];
		Vertex v2 = model->m_Vertices[model->m_Polygons[p].v2];
		Vertex v3 = model->m_Vertices[model->m_Polygons[p].v3];

		v1.pos *= MVP;
		v2.pos *= MVP;
		v3.pos *= MVP;

		render_context->SetCurrentTexture(model->m_Texture);
		render_context->DrawTriangle(v1, v2, v3, DRAW_STYLE);
	}
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow)
{
	if (!Win32::CreateWindow(&win32_window, L"SoftwareRenderer", 1280, 720, AWS_VISIBLE | AWS_DEPTH_BUFFER))
		return -1;

	RenderContext rc(
		win32_window.frame_buffer,
		win32_window.fb_width,
		win32_window.fb_height,
		win32_window.fb_bpp);
	rc.SetClearColor(Color(0.1f, 0.2f, 0.3f));
	rc.SetFlag(CULL_FACES, CULL_FACES_ON);
	rc.SetFlag(DEPTH_TEST, DEPTH_TEST_ON);
	cube.SetName("Cube");
	
	char str[256] = "";
	LARGE_INTEGER current_time, last_time, elapsed_time, frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&last_time);

	while (Win32::HandleMessages()) {
		QueryPerformanceCounter(&current_time);
		elapsed_time.QuadPart = current_time.QuadPart - last_time.QuadPart;
		elapsed_time.QuadPart *= 1000000;
		elapsed_time.QuadPart /= frequency.QuadPart;
		real64 delta_time = elapsed_time.QuadPart / 1000.0f;
		last_time = current_time;

		PERSIST real32 rotX = 0.0f, rotY = 300.0f, rotZ = 0.0f;
		rotY += (real32)delta_time / 50.0f;
		if (fabs(rotX) > 360.0f)
			rotX = 0.0f;
		if (fabs(rotY) > 360.0f)
			rotY = 0.0f;
		if (fabs(rotZ) > 360.0f)
			rotZ = 0.0f;

		char *draw_style;
		if (DRAW_STYLE == DRAW_TRIANGLES)
			draw_style = "Triangles";
		if (DRAW_STYLE == DRAW_LINES)
			draw_style = "Lines";
		if (DRAW_STYLE == DRAW_POINTS)
			draw_style = "Points";
		char *blend_mode = GetBlendModeName(BLEND_MODE);
		rc.SetBlendMode(BLEND_MODE);

		Matrix4f P = PerspectiveMatrix((real32)win32_window.fb_width / (real32)win32_window.fb_height,
			70.0f, 0.1f, 1000.0f);
		Matrix4f t = TranslationMatrix(0.0f, 0.0f, z_position);
		Matrix4f r = RotationMatrix(0.0f, rotY, rotZ);
		r = r * RotationMatrix(-90.0f, 0.0f, 0.0f);
		model->m_TransformationMatrix = t * r;
		Matrix4f MVP = P * model->m_TransformationMatrix;
		
		rc.Clear(FRAME_BUFFER | DEPTH_BUFFER);
			DrawModel(&rc, model, MVP);
			sprintf_s(str, 256,
				"ms: %.2f\n"
				"[F8]\nCull Faces: %s\n"
				"[F7]\nDepth Test: %s\n"
				"[F4][F5][F6]\nDraw Style: %s\n"
				"[F3]\nBlend Mode: %s\n"
				"[F1][F2]\nModel: %s\n"
				"Triangles: %i\n"
				"Pos: [%+.2f][%+.2f][%+.2f]\n"
				"Rot: [%+.2f][%+.2f][%+.2f]\n",
				delta_time,
				rc.GetFlag(CULL_FACES) != 0 ? "true" : "false",
				rc.GetFlag(DEPTH_TEST) != 0 ? "true" : "false",
				draw_style,
				blend_mode,
				model->GetName(),
				model->m_NumPolygons,
				t.a2[0][3], t.a2[1][3], t.a2[2][3],
				rotX, rotY, rotZ);
			DrawString(&rc, str, &font, 6.0f, Color::RED);
		Win32::SwapBuffers(&win32_window);
	}

	Win32::CloseWindow(&win32_window);

	return(0);
}