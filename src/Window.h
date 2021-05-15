#ifndef WINDOW_H_
#include <cstdint>


namespace clb {
	class Window {
	public:
		struct PlatformWindow;

		struct FrameBufferObject {
			void *data;
			int width;
			int height;
			int bpp;
		};

		Window(const char* const title, int width = 720, int height = 480, int x = -1, int y = -1)
				: quit(true), title(title), x(x), y(y), width(width), height(height), platform(nullptr)  {}
		~Window() { Destroy(); }

		int Create();
		void Destroy();
		void Quit() { quit = true; }

		int HandleMessages();
		void Flip();

		const char* const GetTitle() { return title; }
		int GetWidth() { return width; }
		int GetHeight() { return height; }
		FrameBufferObject* GetFrameBuffer() { return &fbo; }

		void SetTitle(const char* const title);
		void ResizeBuffer(int width, int height, int bpp = 0);
		void Resize(int width, int height, int bpp = 0, bool buffer = false);

	private:
		bool quit;
		const char* title;
		int x, y, width, height;
		FrameBufferObject fbo;
		PlatformWindow* platform;
	};
}


#define WINDOW_H_
#endif
