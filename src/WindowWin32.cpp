#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Window.h"

#define WND_CLASS_NAME "SoftwareRenderWindowClass"

LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	clb::Window* window = nullptr;
	if (msg == WM_CREATE) {
		CREATESTRUCTA* cs = (CREATESTRUCTA*)lParam;
		window = (clb::Window*)cs->lpCreateParams;
		SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR)window);
	} else if (window == nullptr) {
		window = (clb::Window*)GetWindowLongPtrA(hWnd, GWLP_USERDATA);
	}

	if (window) {
		LRESULT result = 0;

		switch(msg) {
			case WM_CLOSE: {
				window->Quit();
			} break;

			default: {
				result = DefWindowProcA(hWnd, msg, wParam, lParam);
			} break;
		}

		return result;
	}

	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

namespace clb
{
	struct Window::PlatformWindow {
		HINSTANCE hInst;
		HWND hWnd;
		HDC hDC;
		DWORD dwExStyle;
		DWORD dwStyle;
		BITMAPINFO fboInfo;
	};

	int Window::Create() {
		if (platform)
			return 1;

		platform = new Window::PlatformWindow();

		WNDCLASSEXA wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEXA);
		wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WinProc;
		wcex.hInstance = platform->hInst;
		wcex.lpszClassName = WND_CLASS_NAME;

		if (RegisterClassExA(&wcex)) {
			platform->dwExStyle = 0;
			platform->dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

			RECT rect{};
			rect.right = width;
			rect.top = height;
			AdjustWindowRectEx(&rect, platform->dwStyle, NULL, platform->dwExStyle);

			platform->hWnd = CreateWindowExA(
				platform->dwExStyle, WND_CLASS_NAME, title, platform->dwStyle,
				x < 0 ? CW_USEDEFAULT : 0, y < 0 ? CW_USEDEFAULT : 0,
				rect.right-rect.left, rect.top-rect.bottom,
				NULL, NULL, platform->hInst, this);

			if (platform->hWnd) {
				platform->hDC = GetDC(platform->hWnd);
				ResizeBuffer(width, height, 32);
				GetWindowRect(platform->hWnd, &rect);
				x = rect.left;
				y = rect.top;
				quit = false;
				return 0;
			}
		}

		Destroy();
		return -1;
	}

	void Window::Destroy() {
		if (fbo.data) {
			VirtualFree(fbo.data, NULL, MEM_RELEASE);
			fbo.data = nullptr;
		}

		if (platform) {
			if (platform->hDC) ReleaseDC(platform->hWnd, platform->hDC);
			if (platform->hWnd) DestroyWindow(platform->hWnd);

			delete platform;
			platform = nullptr;
		}
	}

	int Window::HandleMessages() {
		MSG msg = {};
		if (quit)
			return 1;

		while (PeekMessageA(&msg, platform->hWnd, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				return 2;

			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		return 0;
	}

	void Window::Flip() {
		StretchDIBits(platform->hDC,
			0, 0, width, height,
			0, 0, fbo.width, fbo.height,
			fbo.data, &platform->fboInfo, DIB_RGB_COLORS, SRCCOPY);
	}

	void Window::ResizeBuffer(int width, int height, int bpp) {
		if (fbo.data) VirtualFree(fbo.data, NULL, MEM_RELEASE);
		if (width > 0) fbo.width = width;
		if (height > 0) fbo.height = height;
		if (bpp > 0) fbo.bpp = bpp;

		platform->fboInfo.bmiHeader.biSize = sizeof(BITMAPINFO);
		platform->fboInfo.bmiHeader.biWidth = fbo.width;
		platform->fboInfo.bmiHeader.biHeight = fbo.height;
		platform->fboInfo.bmiHeader.biPlanes = 1;
		platform->fboInfo.bmiHeader.biBitCount = fbo.bpp;
		platform->fboInfo.bmiHeader.biCompression = BI_RGB;

		fbo.data = VirtualAlloc(0, fbo.width * fbo.height * fbo.bpp, MEM_COMMIT, PAGE_READWRITE);
	}

	void Window::SetTitle(const char* const title) {
		this->title = title;
		SetWindowTextA(platform->hWnd, title);
	}

	void Window::Resize(int width, int height, int bpp, bool buffer) {
		if(width > 0) this->width = width;
		if(height > 0) this->height = height;
		if (buffer) ResizeBuffer(this->width, this->height, bpp);
		
		RECT rect{};
		rect.right = this->width;
		rect.top = this->height;
		AdjustWindowRectEx(&rect, platform->dwStyle, NULL, platform->dwExStyle);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		SetWindowPos(platform->hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
	}
}
