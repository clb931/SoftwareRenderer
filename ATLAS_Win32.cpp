#include "stdafx.h"
#include "ATLAS_Win32.h"


LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ATLAS
{
	Window::Window(wstring title, uint32 style, uint32 width, uint32 height, uint32 buffer_width, uint32 buffer_height, uint32 buffer_bpp)
	{
		m_BufferInfo = { 0 };
		Create(title, style, width, height, buffer_width, buffer_height, buffer_bpp);
	}

	Window::~Window()
	{
		Destroy();
	}

	bool32 Window::Create(wstring title, uint32 style, uint32 width, uint32 height, uint32 buffer_width, uint32 buffer_height, uint32 buffer_bpp)
	{
		m_Title = title;
		LPCWSTR lpCLassName = m_Title.c_str();

		WNDCLASSEXW wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WinProc;
		wcex.hInstance = m_hInst;
		wcex.lpszClassName = lpCLassName;

		if (RegisterClassExW(&wcex)) {
			DWORD dwExStyle = 0;
			LPCWSTR lpWindowdowName = m_Title.c_str();
			DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

			m_hWnd = CreateWindowExW(
				dwExStyle,
				lpCLassName, lpWindowdowName,
				dwStyle,
				CW_USEDEFAULT, CW_USEDEFAULT,
				width, height,
				NULL, NULL,
				m_hInst, NULL);

			if (m_hWnd)
			{
				m_hDC = GetDC(m_hWnd);

				RECT window_rect;
				GetWindowRect(m_hWnd, &window_rect);

				m_Width = window_rect.right - window_rect.left;
				m_Height = window_rect.bottom - window_rect.top;
				m_Style = style;

				if (buffer_width == CW_USEDEFAULT)
					buffer_width = m_Width;
				if (buffer_height == CW_USEDEFAULT)
					buffer_height = m_Height;

				ResizeBuffer(buffer_width, buffer_height, buffer_bpp);

				return TRUE;
			}
		}

		Destroy();
		return FALSE;
	}

	void Window::Destroy()
	{
		if (m_FrameBuffer) {
			VirtualFree(m_FrameBuffer, NULL, MEM_RELEASE);
			m_FrameBuffer = nullptr;
		}

		if (m_hRC) {
			delete m_hRC;
			m_hRC = nullptr;
		}

		if (m_hDC) {
			ReleaseDC(m_hWnd, m_hDC);
			m_hDC = NULL;
		}

		if (m_hWnd) {
			DestroyWindow(m_hWnd);
			m_hWnd = NULL;
		}

		m_Title 		= L"Unititled Windowdow";
		m_BufferInfo	= { 0 };
		m_BufferWidth	= 0;
		m_BufferHeight	= 0;
		m_BufferBPP		= 0;
		m_Style			= 0;
		m_Width 		= 0;
		m_Height 		= 0;
	}

	HINSTANCE Window::GetInstance()
	{
		return m_hInst;
	}

	HWND Window::GetHandle()
	{
		return m_hWnd;
	}

	HDC Window::GetHDC()
	{
		return m_hDC;
	}

	void* Window::GetHRC()
	{
		return m_hRC;
	}

	bool32 Window::HandleMessages()
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

	void Window::Flip()
	{
		StretchDIBits(m_hDC,
			0, 0, m_Width, m_Height,
			0, 0, m_BufferWidth, m_BufferHeight,
			m_FrameBuffer, &m_BufferInfo, DIB_RGB_COLORS, SRCCOPY);
	}

	void *Window::GetBuffer()
	{
		return m_FrameBuffer;
	}

	uint32 Window::GetBufferWidth()
	{
		return m_BufferWidth;
	}

	uint32 Window::GetBufferHeight()
	{
		return m_BufferHeight;
	}

	uint32 Window::GetBufferBPP()
	{
		return m_BufferBPP;
	}

	uint32 Window::GetBufferSize(uint32 &width, uint32 &height, uint32 &bpp)
	{
		width = m_BufferWidth;
		height = m_BufferHeight;
		bpp = m_BufferBPP;

		return width * height * bpp;
	}

	wstring Window::GetTitle()
	{
		return m_Title;
	}

	void Window::GetMousePos(int32 &x, int32 &y, int32 &f)
	{
		x = m_MouseX;
		y = m_MouseY;
		f = m_MouseF;
	}

	uint32 Window::GetStyle()
	{
		return m_Style;
	}

	uint32 Window::GetWidth()
	{
		return m_Width;
	}

	uint32 Window::GetHeight()
	{
		return m_Height;
	}

	uint32 Window::GetSize(uint32 &width, uint32 &height)
	{
		width = m_Width;
		height = m_Height;

		return width * height;
	}

	void Window::SetRC(void *RC)
	{
		m_hRC = RC;
	}

	void Window::ResizeBuffer(uint32 width, uint32 height, uint32 bpp)
	{
		if (m_FrameBuffer)
			VirtualFree(m_FrameBuffer, NULL, MEM_RELEASE);

		m_BufferWidth = width;
		m_BufferHeight = height;
		if (bpp) m_BufferBPP = bpp;

		m_BufferInfo.bmiHeader.biSize = sizeof(m_BufferInfo.bmiHeader);
		m_BufferInfo.bmiHeader.biWidth = m_BufferWidth;
		m_BufferInfo.bmiHeader.biHeight = m_BufferHeight;
		m_BufferInfo.bmiHeader.biPlanes = 1;
		m_BufferInfo.bmiHeader.biBitCount = m_BufferBPP;
		m_BufferInfo.bmiHeader.biCompression = BI_RGB;

		m_FrameBuffer = VirtualAlloc(0, m_BufferWidth * m_BufferHeight * m_BufferBPP, MEM_COMMIT, PAGE_READWRITE);
	}

	void Window::SetTitle(wstring title)
	{
		m_Title = title;
	}

	void Window::SetMousePos(int32 x, int32 y, int32 f)
	{
		m_MouseX = x;
		m_MouseY = y;
		m_MouseF = f;
	}

	void Window::SetStyle(uint32 style)
	{
		m_Style = style;
	}

	void Window::Resize(uint32 width, uint32 height, uint32 bpp)
	{
		m_Width = width;
		m_Height = height;
		if(bpp) m_BufferBPP = bpp;

		ResizeBuffer(m_Width, m_Height, m_BufferBPP);
	}
}