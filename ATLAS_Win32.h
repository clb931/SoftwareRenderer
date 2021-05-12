#ifndef ATLAS_WIN32_H


namespace ATLAS
{
	class Window
	{
	public:
		Window() :
			m_hInst(GetModuleHandleW(NULL)),
			m_hWnd(NULL),
			m_hDC(NULL),
			m_hRC(nullptr),
			m_Title(L"Untitled Windowdow"),
			m_FrameBuffer(nullptr),
			m_BufferWidth(0),
			m_BufferHeight(0),
			m_BufferBPP(0),
			m_Style(0),
			m_Width(0),
			m_Height(0)
		{ m_BufferInfo = { 0 }; }
		Window(wstring title, uint32 style, uint32 width = 720, uint32 height = 480, uint32 buffer_width = CW_USEDEFAULT, uint32 buffer_height = CW_USEDEFAULT, uint32 buffer_bpp = 32);
		~Window();

		bool32 Create(wstring title, uint32 style, uint32 width = 720, uint32 height = 480, uint32 buffer_width = CW_USEDEFAULT, uint32 buffer_height = CW_USEDEFAULT, uint32 buffer_bpp = 32);
		void Destroy();

		HINSTANCE 	GetInstance();
		HWND 		GetHandle();
		HDC 		GetHDC();
		void*		GetHRC();

		bool32		HandleMessages();
		void		Flip();

		void		*GetBuffer();
		uint32		GetBufferWidth();
		uint32		GetBufferHeight();
		uint32		GetBufferBPP();
		uint32		GetBufferSize(uint32 &width, uint32 &height, uint32 &bpp);

		wstring		GetTitle();
		void 		GetMousePos(int32 &x, int32 &y, int32 &f);

		uint32		GetStyle();
		uint32		GetWidth();
		uint32		GetHeight();
		uint32		GetSize(uint32 &width, uint32 &height);

		void 		SetRC(void *RC);

		void		ResizeBuffer(uint32 width, uint32 height, uint32 bpp = 0);

		void		SetTitle(std::wstring title);
		void		SetMousePos(int32 x, int32 y, int32 f);

		void		SetStyle(uint32 style);
		void 		Resize(uint32 width, uint32 height, uint32 bpp = 0, bool buffer = false);

	private:
		HINSTANCE		m_hInst;
		HWND			m_hWnd;
		HDC				m_hDC;
		void*			m_hRC;
		std::wstring	m_Title;
		int32			m_MouseX;
		int32			m_MouseY;
		int32			m_MouseF;
		BITMAPINFO		m_BufferInfo;
		void			*m_FrameBuffer;
		uint32			m_BufferWidth;
		uint32			m_BufferHeight;
		uint32			m_BufferBPP;
		uint32			m_Style;
		uint32			m_Width;
		uint32			m_Height;
	};
}


#define ATLAS_WIN32_H
#endif