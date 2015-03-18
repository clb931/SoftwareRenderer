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

GLOBAL ATLAS::Win32::Window win32_window = {};

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

Vector4f verts[]
{
	Vector4f(-0.5f, -0.5f, 0.5f),	//left  - top    - front
		Vector4f(0.5f, -0.5f, 0.5f),	//right - top    - front
		Vector4f(0.5f, -0.5f, -0.5f),	//left  - bottom - front
		Vector4f(-0.5f, -0.5f, -0.5f),	//right - bottom - front
		Vector4f(-0.5f, 0.5f, 0.5f),	//left  - top    - back
		Vector4f(0.5f, 0.5f, 0.5f),		//right - top    - back
		Vector4f(0.5f, 0.5f, -0.5f),	//left  - bottom - back
		Vector4f(-0.5f, 0.5f, -0.5f)	//right - bottom - back
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
Color colors[]
{
	Color::RED, Color::GREEN, Color::BLUE, Color::GREEN,
		Color::BLUE, Color::GREEN, Color::RED, Color::GREEN
};
UV uvs[]
{
	UV(0.0f, 0.0f),		// v0
		UV(1.0f, 0.0f),		// v1
		UV(1.0f, 0.0f),		// v2
		UV(0.0f, 0.0f),		// v3
		UV(0.0f, 1.0f),		// v4
		UV(1.0f, 1.0f),		// v5
		UV(1.0f, 1.0f),		// v6
		UV(0.0f, 1.0f),		// v7
};

Texture cubetexture("texture.bmp");
Texture spaceshiptexture("spaceshiptexture.bmp");
Model spaceship("spaceship.3DS", &spaceshiptexture);
Model cube(verts, 8, polys, 12, uvs, &cubetexture, colors);

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
	rc.SetFlag(CULL_FACES, true);

	Matrix4f P = PerspectiveMatrix(
		(real32)win32_window.fb_width / (real32)win32_window.fb_height,
		70.0f, 0.1f, 1000.0f);

	//Vertex v1(
	//	Vector4f(-1, -1, 0),
	//	UV(0.0f, 0.0f),
	//	Color::GREEN);
	//Vertex v2(
	//	Vector4f(0, 1, 0),
	//	UV(0.5f, 1.0f),
	//	Color::RED);
	//Vertex v3(
	//	Vector4f(1, -1, 0),
	//	UV(1.0f, 0.0f),
	//	Color::BLUE);

	LARGE_INTEGER current_time, last_time, frequency;
	QueryPerformanceFrequency(&frequency);
	int64 counter_frequency = frequency.QuadPart;
	QueryPerformanceCounter(&last_time);

	while (Win32::HandleMessages()) {
		QueryPerformanceCounter(&current_time);
		int64 elapsed_time = current_time.QuadPart - last_time.QuadPart;
		real64 delta_time = ((1000.0f * (real64)elapsed_time) / (real64)counter_frequency);
		last_time = current_time;

		PERSIST real32 rotY = 0.0f;
		rotY += (real32)delta_time / 10.0f;

		Matrix4f t = TranslationMatrix(0.0f, 0.0f, 300.0f);
		Matrix4f r = RotationMatrix(0.0f, rotY, 0.0f);
		Matrix4f MV = t * r;
		Matrix4f MVP = P * MV;
		
		rc.Clear(COLOR_BUFFER | DEPTH_BUFFER);
		for (uint32 p = 0; p < spaceship.m_NumPolygons; ++p) {
			rc.DrawTriangle(
				MVP * spaceship.m_Vertices[spaceship.m_Polygons[p].v1],
				Color::WHITE,
				spaceship.m_UVs[spaceship.m_Polygons[p].v1],
				MVP * spaceship.m_Vertices[spaceship.m_Polygons[p].v2],
				Color::WHITE,
				spaceship.m_UVs[spaceship.m_Polygons[p].v2],
				MVP * spaceship.m_Vertices[spaceship.m_Polygons[p].v3],
				Color::WHITE,
				spaceship.m_UVs[spaceship.m_Polygons[p].v3],
				spaceship.m_Texture);
		}
		Win32::SwapBuffers(&win32_window);
	}

	Win32::CloseWindow(&win32_window);

	return(0);
}