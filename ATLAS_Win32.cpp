#include "ATLAS_Stdafx.h"
#include "ATLAS_RenderEngine.h"

#include <Windows.h>
#undef CreateWindow

GLOBAL bool32 g_Quit	= FALSE;

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
			BITMAPINFO		back_buffer_info;
			ATLAS::Window	game_window;
		};

		void ResizeScreenBuffers(Win32::Window *win32_window, int32 width, int32 height, int32 bpp = 4)
		{
			ATLAS::Window		*window = &win32_window->game_window;
			BITMAPINFO			*back_buffer_info = &win32_window->back_buffer_info;
			ATLAS::ScreenBuffer *back_buffer = &window->back_buffer;
			ATLAS::ScreenBuffer *depth_buffer = &window->depth_buffer;

			if (back_buffer->pixels)
				VirtualFree(back_buffer->pixels, NULL, MEM_RELEASE);

			back_buffer->width = width;
			back_buffer->height = height;
			back_buffer->bytes_per_pixel = sizeof(uint32);
			back_buffer->pitch = back_buffer->width * back_buffer->bytes_per_pixel;
			back_buffer->size = (back_buffer->width * back_buffer->height) * back_buffer->bytes_per_pixel;

			back_buffer_info->bmiHeader.biSize = sizeof(back_buffer_info->bmiHeader);
			back_buffer_info->bmiHeader.biWidth = back_buffer->width;
			back_buffer_info->bmiHeader.biHeight = back_buffer->height;
			back_buffer_info->bmiHeader.biPlanes = 1;
			back_buffer_info->bmiHeader.biBitCount = 8 * back_buffer->bytes_per_pixel;
			back_buffer_info->bmiHeader.biCompression = BI_RGB;

			back_buffer->pixels = VirtualAlloc(0, back_buffer->size, MEM_COMMIT, PAGE_READWRITE);
			

			if (win32_window->game_window.style & AWS_DEPTH_BUFFER)
			{
				if (depth_buffer->pixels)
					VirtualFree(depth_buffer->pixels, NULL, MEM_RELEASE);

				depth_buffer->width = width;
				depth_buffer->height = height;
				depth_buffer->bytes_per_pixel = sizeof(real32);
				depth_buffer->pitch = depth_buffer->width * depth_buffer->bytes_per_pixel;
				depth_buffer->size = (depth_buffer->width * depth_buffer->height) * depth_buffer->bytes_per_pixel;

				depth_buffer->pixels = VirtualAlloc(0, depth_buffer->size, MEM_COMMIT, PAGE_READWRITE);
			}
		}

		void FlipScreenBuffers(Win32::Window *win32_window)
		{
			ATLAS::Window		*window = &win32_window->game_window;
			BITMAPINFO			*back_buffer_info = &win32_window->back_buffer_info;
			ATLAS::ScreenBuffer *back_buffer = &window->back_buffer;

			RECT rc = {};
			GetWindowRect(win32_window->hwnd, &rc);
			window->width = rc.right - rc.left;
			window->height = rc.bottom - rc.top;

			StretchDIBits(win32_window->hdc,
				0, 0, window->width, window->height,
				0, 0, back_buffer->width, back_buffer->height,
				back_buffer->pixels, back_buffer_info,
				DIB_RGB_COLORS, SRCCOPY);
		};

		bool32 CreateWindow(Win32::Window *win32_window, LPCWSTR title, int32 window_width = CW_USEDEFAULT, int32 window_height = CW_USEDEFAULT, int32 style = AWS_VISIBLE | AWS_DOUBLE_BUFFER, int32 buffer_width = 1280, int32 buffer_height = 720)
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

				ATLAS::Window *window = &win32_window->game_window;
				window->width = rc.right - rc.left;
				window->height = rc.bottom - rc.top;
				window->style = style;

				ResizeScreenBuffers(win32_window, buffer_width, buffer_height);

				return TRUE;
			}

			return FALSE;
		}

		void CloseWindow(Win32::Window *win32_window)
		{
			DestroyWindow(win32_window->hwnd);

			ATLAS::Window *window = &win32_window->game_window;

			if (&window->back_buffer.pixels)
				VirtualFree(&window->back_buffer.pixels, NULL, MEM_RELEASE);
			if (&window->depth_buffer.pixels)
				VirtualFree(&window->depth_buffer.pixels, NULL, MEM_RELEASE);

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
			ATLAS::Win32::FlipScreenBuffers(&win32_window);
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

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow)
{
	if (!ATLAS::Win32::CreateWindow(
		&win32_window,
		L"SoftwareRenderer",
		1280, 720,
		ATLAS::AWS_VISIBLE | ATLAS::AWS_DEPTH_BUFFER,
		1280, 720))
		return -1;

	ATLAS::RenderEngine render_engine(&win32_window.game_window);
	render_engine.SetClearColor(ATLAS::Color(0.1f, 0.2f, 0.3f));
	render_engine.SetFlag(ATLAS::CULL_FACES, true);
	render_engine.SetFlag(ATLAS::ORTHOGRAPHIC_PROJECTION, false);
	render_engine.SetProjection((real32)win32_window.game_window.back_buffer.width /
								(real32)win32_window.game_window.back_buffer.height,
								30.0f, 1.0f, 10000.0f);

	while (!g_Quit) {
		MSG msg = {};
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				g_Quit = TRUE;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		render_engine.DrawScene();

		ATLAS::Win32::FlipScreenBuffers(&win32_window);
	}

	ATLAS::Win32::CloseWindow(&win32_window);

	return(0);
}

namespace ATLAS
{
	Vector4f verts[]
	{
		Vector4f(-0.5f, -0.5f, 0.5f), //left  - top    - front
		Vector4f(0.5f, -0.5f, 0.5f), //right - top    - front
		Vector4f(0.5f, -0.5f, -0.5f), //left  - bottum - front
		Vector4f(-0.5f, -0.5f, -0.5f), //right - bottum - front
		Vector4f(-0.5f, 0.5f, 0.5f), //left  - top    - back
		Vector4f(0.5f, 0.5f, 0.5f), //right - top    - back
		Vector4f(0.5f, 0.5f, -0.5f), //left  - bottum - back
		Vector4f(-0.5f, 0.5f, -0.5f) //right - bottum - back
	};

	Polygon polys[]
	{
		Polygon(0, 1, 4), //front
		Polygon(4, 1, 5), //front
		Polygon(1, 2, 5), //bottum
		Polygon(2, 6, 5), //bottum
		Polygon(2, 3, 6), //back
		Polygon(3, 7, 6), //back
		Polygon(3, 0, 7), //top
		Polygon(0, 4, 7), //top
		Polygon(4, 5, 7), //right
		Polygon(5, 6, 7), //right
		Polygon(3, 2, 0), //left
		Polygon(2, 1, 0) //left
	};

	Color colors[]
	{
		RED, GREEN, BLUE, GREEN,
		BLUE, GREEN, RED, GREEN
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

	void RenderEngine::DrawScene()
	{
		Clear(COLOR_BUFFER | DEPTH_BUFFER);
		
		PERSIST real32 x = 0.0f, y = 0.0f, z = 0.0f, r = 0.0f;
		x -= 0.01f;
		y -= 0.01f;
		z -= 0.01f;
		r += 0.5f;

		Matrix4f T = TranslationMatrix(0.0f, 0.025f, -10.0f);
		Matrix4f Rx = T * RotationMatrix(r * 2.0f, 0.0f, 0.0f);
		Matrix4f Ry = Rx * RotationMatrix(0.0f, r, 0.0f);
		Matrix4f Rz = Ry * RotationMatrix(0.0f, 0.0f, r / 2.0f);
		cube.m_TransformationMatrix = Rz * ScaleMatrix(1.0f, 1.0f, 1.0f);
		DrawModel(&cube, DRAW_TRIANGLES);

		T = TranslationMatrix(0.0f, 0.025f, -100.0f);
		Rx = T * RotationMatrix(r * 2.0f, 0.0f, 0.0f);
		Ry = Rx * RotationMatrix(0.0f, r, 0.0f);
		Rz = Ry * RotationMatrix(0.0f, 0.0f, r / 2.0f);
		spaceship.m_TransformationMatrix = Rz * ScaleMatrix(0.1f, 0.1f, 0.1f);
		DrawModel(&spaceship, DRAW_TRIANGLES);
	}
}