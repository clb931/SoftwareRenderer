#include "stdafx.h"
#include "ATLAS_Model.h"
#include "ATLAS_RenderContext.h"
#include "ATLAS_Texture.h"
#include "ATLAS_Win32.h"
#include "ATLAS_Entity.h"
#include "ATLAS_Shader.h"

struct Font {
	ATLAS::Texture* tex;
	uint32 width, height;

	Font() : tex(nullptr) {}
	Font(ATLAS::Texture* tex, uint32 width, uint32 height)
		: tex(tex), width(width), height(height)
	{}
};

INTERN ATLAS::Window window;
INTERN ATL::Rasterizer rasterizer;
INTERN int32 font_index = 0;
INTERN Font font[2];
INTERN int32 model_index = 0;
INTERN LARGE_INTEGER freq;

void InitTimer()
{
	QueryPerformanceFrequency(&freq);
}

double GetMicroseconds()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	double time = t.QuadPart * 1000000;
	time /= static_cast<double>(freq.QuadPart);
	return time;
}

double GetMilliseconds()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	double time = t.QuadPart * 1000;
	time /= static_cast<double>(freq.QuadPart);
	return time;
}

double GetSeconds()
{
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);
	double time = t.QuadPart;
	time /= static_cast<double>(freq.QuadPart);
	return time;
}

LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (msg) {
		case WM_SIZE: {
#ifdef RESIZE_BUFFERS
			bool resize = true;
#else
			bool resize = false;
#endif
			window.Resize(LOWORD(lParam), HIWORD(lParam), 0, resize);
			window.Flip();
		} break;

		case WM_KEYDOWN: {
			if (wParam == VK_ESCAPE) {
				PostQuitMessage(0);
			}
			if (wParam == VK_F7) {
				if (font_index == 1)
					font_index = 0;
				else
					font_index = 1;
			}
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
	if (!window.Create(L"SoftwareRenderer", NULL))
		return -1;

	ATLAS::Texture font0("font0.bmp");
	ATLAS::Texture font1("font1.bmp");
	font[0] = Font(&font0, 10, 10);
	font[1] = Font(&font1, 16, 16);

	ATLAS::Vertex verts[]
	{
		ATLAS::Vertex(ATLAS::Vector3f(-0.5f, -0.5f, 0.5f),	ATLAS::UV(0.0f, 0.0f), ATLAS::Color::YELLOW),	// left  - bottom	- back
		ATLAS::Vertex(ATLAS::Vector3f(0.5f, -0.5f, 0.5f),	ATLAS::UV(1.0f, 0.0f), ATLAS::Color::WHITE),	// right - bottom	- back
		ATLAS::Vertex(ATLAS::Vector3f(0.5f, -0.5f, -0.5f),	ATLAS::UV(1.0f, 0.0f), ATLAS::Color::CYAN),		// left  - bottom	- front
		ATLAS::Vertex(ATLAS::Vector3f(-0.5f, -0.5f, -0.5f),	ATLAS::UV(0.0f, 0.0f), ATLAS::Color::GREEN),	// right - bottom	- front
		ATLAS::Vertex(ATLAS::Vector3f(-0.5f, 0.5f, 0.5f),	ATLAS::UV(0.0f, 1.0f), ATLAS::Color::RED),		// left  - top		- back
		ATLAS::Vertex(ATLAS::Vector3f(0.5f, 0.5f, 0.5f),	ATLAS::UV(1.0f, 1.0f), ATLAS::Color::MAGENTA),	// right - top		- back
		ATLAS::Vertex(ATLAS::Vector3f(0.5f, 0.5f, -0.5f),	ATLAS::UV(1.0f, 1.0f), ATLAS::Color::BLUE), 	// left  - top		- front
		ATLAS::Vertex(ATLAS::Vector3f(-0.5f, 0.5f, -0.5f),	ATLAS::UV(0.0f, 1.0f), ATLAS::Color::BLACK)		// right - top		- front
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
		ATLAS::Polygon(2, 1, 0)		//left
	};

	ATLAS::Texture cubetexture("texture.bmp");
	ATLAS::Model cube("Cube", verts, 8, polys, 12);
	ATLAS::Texture spaceshiptexture("spaceshiptexture.bmp");
	ATLAS::Model spaceship("spaceship.3DS");
	ATLAS::Model monkey("monkey.3DS");

	ATLAS::Entity entity(&cube, nullptr);

	char str[512] = "";
	InitTimer();
	double time = GetMilliseconds();

	while (window.HandleMessages()) {
		double t = GetMilliseconds();
		double delta_time = t - time;
		time = t;

		const double dt = 1.0 / 60.0;
		PERSIST double frame_accumulator = 0.0;
		PERSIST ATLAS::Vector3f rot;
		frame_accumulator += delta_time / 1000.0;
		while (frame_accumulator >= dt) {
			rot.x += dt * 10.0f;
			rot.y += dt * 20.0f;
			rot.z += dt * 30.0f;
			frame_accumulator -= dt;
		}

		ATL::Rasterizer::Uniforms uniforms{};
		uniforms.buffer.data = window.GetBuffer();
		uniforms.buffer.width = window.GetBufferWidth();
		uniforms.buffer.height = window.GetBufferHeight();
		uniforms.buffer.bpp = window.GetBufferBPP();
		uniforms.screenTransform = ATLAS::ScreenSpaceMatrix4f(uniforms.buffer.width, uniforms.buffer.height);

		ATL::Rasterizer::Inputs rastIn{};
		rastIn.polygonCount = entity.getModel()->m_NumPolygons;
		rastIn.polygons = entity.getModel()->m_Polygons;
		rastIn.vertexCount = entity.getModel()->m_NumVertices;
		rastIn.vertices = entity.getModel()->m_Vertices;

		entity.setRotation(rot);
		// entity.getPosition().z = -2.0f;
		ATLAS::Matrix4f M = entity.getTransformationMatrix();
		ATLAS::Matrix4f V = ATLAS::TranslationMatrix4f(0.0f, 0.0f, 3.0f);
		ATLAS::Matrix4f P = ATLAS::PerspectiveMatrix4f(
			(real32)uniforms.buffer.width / (real32)uniforms.buffer.height,
			70.0f, 0.1f, 1000.0f);
		uniforms.vert.getUniforms().MVP = P * V * M;

		rasterizer.setUniforms(uniforms);
		rasterizer.clear();
		rasterizer.run(rastIn);

		PERSIST double frame_time = 0;
		frame_time = lerp(delta_time, frame_time, 0.95);
		printf("fps: %i dt: %.2f ms\n", (int)(1.0 / (frame_time / 1000.0)), frame_time);
		// sprintf_s(str, 512,
		// 	"fps: %i dt: %.2f ms\n"
		// 	"[F1][F2]Model:  %s\n"
		// 	"[F3]Draw Style: %s\n"
		// 	"[F4]Blend Mode: %s\n"
		// 	"[F5]Depth Test: %s\n"
		// 	"[F6]Cull Faces: %s\n"
		// 	"[F7]Font:   %i\n"
		// 	"Point Size: %i\n"
		// 	"Triangles:  %i\n"
		// 	"Pos X: %+.2f\n"
		// 	"Pos Y: %+.2f\n"
		// 	"Pos Z: %+.2f\n"
		// 	"Rot X: %+.2f\n"
		// 	"Rot Y: %+.2f\n"
		// 	"Rot Z: %+.2f\n",
		// 	(int)(1.0 / (frame_time / 1000.0)), frame_time,
		// 	model[model_index]->GetName(),
		// 	draw_style,
		// 	GetBlendModeName(pRC->GetBlendMode()),
		// 	pRC->GetFlag(ATLAS::DEPTH_TEST) != 0 ? "true" : "false",
		// 	pRC->GetFlag(ATLAS::CULL_FACES) != 0 ? "true" : "false",
		// 	font_index, pRC->GetPointSize(),
		// 	model[model_index]->m_NumPolygons,
		// 	model[model_index]->m_TransformationMatrix.a2[0][3],
		// 	model[model_index]->m_TransformationMatrix.a2[1][3],
		// 	model[model_index]->m_TransformationMatrix.a2[2][3],
		// 	rotX, rotY, rotZ);
		window.Flip();
	}

	return(0);
}