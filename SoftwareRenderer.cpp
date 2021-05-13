#include "stdafx.h"
#include "ATLAS_Model.h"
#include "ATLAS_RenderContext.h"
#include "ATLAS_Texture.h"
#include "ATLAS_Win32.h"
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
INTERN ATLAS::Model *model[3];
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
			// if (wParam == VK_F1) {
			// 	if (model_index == 0) {
			// 		model_index = 2;
			// 		ATLAS::Matrix4f T = ATLAS::TranslationMatrix4f(0.0f, 0.0f, -4.0f);
			// 		ATLAS::Matrix4f R = ATLAS::RotationMatrix4f(-90.0f, 0.0f, 0.0f);
			// 		model[model_index]->m_TransformationMatrix = T * R;
			// 	}
			// 	else if (model_index == 1) {
			// 		model_index--;
			// 		ATLAS::Matrix4f T = ATLAS::TranslationMatrix4f(0.0f, 0.0f, -3.0f);
			// 		ATLAS::Matrix4f R = ATLAS::RotationMatrix4f(0.0f, 0.0f, 0.0f);
			// 		model[model_index]->m_TransformationMatrix = T * R;
			// 	}
			// 	else if (model_index == 2) {
			// 		model_index--;
			// 		ATLAS::Matrix4f T = ATLAS::TranslationMatrix4f(0.0f, 0.0f, -300.0f);
			// 		ATLAS::Matrix4f R = ATLAS::RotationMatrix4f(-90.0f, 0.0f, 0.0f);
			// 		model[model_index]->m_TransformationMatrix = T * R;
			// 	}
			// }
			// if (wParam == VK_F2) {
			// 	if (model_index == 0) {
			// 		model_index++;
			// 		ATLAS::Matrix4f T = ATLAS::TranslationMatrix4f(0.0f, 0.0f, -300.0f);
			// 		ATLAS::Matrix4f R = ATLAS::RotationMatrix4f(-90.0f, 0.0f, 0.0f);
			// 		model[model_index]->m_TransformationMatrix = T * R;
			// 	}
			// 	else if (model_index == 1) {
			// 		model_index++;
			// 		ATLAS::Matrix4f T = ATLAS::TranslationMatrix4f(0.0f, 0.0f, -4.0f);
			// 		ATLAS::Matrix4f R = ATLAS::RotationMatrix4f(-90.0f, 0.0f, 0.0f);
			// 		model[model_index]->m_TransformationMatrix = T * R;
			// 	}
			// 	else if (model_index == 2) {
			// 		model_index = 0;
			// 		ATLAS::Matrix4f T = ATLAS::TranslationMatrix4f(0.0f, 0.0f, -3.0f);
			// 		ATLAS::Matrix4f R = ATLAS::RotationMatrix4f(0.0f, 0.0f, 0.0f);
			// 		model[model_index]->m_TransformationMatrix = T * R;
			// 	}
			// }
			// if (wParam == VK_F3) {
			// 	ATLAS::AtlasEnum draw_style = (ATLAS::AtlasEnum)(pRC->GetDrawStyle() + 1);
			// 	if (draw_style > ATLAS::DRAW_TRIANGLES)
			// 		draw_style = ATLAS::DRAW_POINTS;

			// 	pRC->SetDrawStyle(draw_style);
			// }
			// if (wParam == VK_F4) {
			// 	ATLAS::BlendMode blend_mode = (ATLAS::BlendMode)(pRC->GetBlendMode() + 1);
			// 	if (blend_mode > ATLAS::BLEND_LIGHTEN)
			// 		blend_mode = ATLAS::BLEND_NORMAL;

			// 	pRC->SetBlendMode(blend_mode);
			// }
			// if (wParam == VK_F5) {
			// 	pRC->SetFlag(ATLAS::DEPTH_TEST, !pRC->GetFlag(ATLAS::DEPTH_TEST));
			// }
			// if (wParam == VK_F6) {
			// 	pRC->SetFlag(ATLAS::CULL_FACES, !pRC->GetFlag(ATLAS::CULL_FACES));
			// }
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
	ATLAS::Model cube("Cube", verts, 8, polys, 12, &cubetexture);
	cube.m_TransformationMatrix = ATLAS::IdentityMatrix4f;
	ATLAS::Texture spaceshiptexture("spaceshiptexture.bmp");
	ATLAS::Model spaceship("spaceship.3DS", &spaceshiptexture);
	ATLAS::Model monkey("monkey.3DS", &spaceshiptexture);
	model[0] = &cube;
	model[1] = &spaceship;
	model[2] = &monkey;

	char str[512] = "";
	InitTimer();
	double time = GetMilliseconds();

	while (window.HandleMessages()) {
		double t = GetMilliseconds();
		double delta_time = t - time;
		time = t;

		const double dt = 1.0 / 60.0;
		PERSIST double frame_accumulator = 0.0;
		PERSIST real32 rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
		frame_accumulator += delta_time / 1000.0;
		while (frame_accumulator >= dt) {
			rotX += dt * 10.0f;
			if (fabs(rotX) > 360.0f)
				rotX = 0.0f;

			rotY += dt * 20.0f;
			if (fabs(rotY) > 360.0f)
				rotY = 0.0f;
			
			rotZ += dt * 1.0f;
			if (fabs(rotZ) > 360.0f)
				rotZ = 0.0f;

			frame_accumulator -= dt;
		}

		ATL::Rasterizer::Uniforms uniforms{};
		uniforms.buffer.data = window.GetBuffer();
		uniforms.buffer.width = window.GetBufferWidth();
		uniforms.buffer.height = window.GetBufferHeight();
		uniforms.buffer.bpp = window.GetBufferBPP();
		uniforms.screenTransform = ATLAS::ScreenSpaceMatrix4f(uniforms.buffer.width, uniforms.buffer.height);

		ATL::Rasterizer::Inputs rastIn{};
		rastIn.polygonCount = model[model_index]->m_NumPolygons;
		rastIn.polygons = model[model_index]->m_Polygons;
		// rastIn.polygons = new ATLAS::Polygon[rastIn.polygonCount];
		// for (int i = 0; i < rastIn.polygonCount; ++i) {
		// 	rastIn.polygons[i] = model[model_index]->m_Polygons[i];
		// }
		rastIn.vertexCount = model[model_index]->m_NumVertices;
		rastIn.vertices = model[model_index]->m_Vertices;
		// rastIn.vertices = new ATLAS::Vertex[rastIn.vertexCount];
		// for (int i = 0; i < rastIn.vertexCount; ++i) {
		// 	rastIn.vertices[i] = model[model_index]->m_Vertices[i];
		// }

		model[model_index]->m_TransformationMatrix
			= model[model_index]->m_TransformationMatrix
			* ATLAS::TranslationMatrix4f(0.0f, 0.0f, 0.0f)
			* ATLAS::RotationMatrix4f(rotX, rotY, rotZ)
			* ATLAS::ScaleMatrix4f(1.0f, 1.0f, 1.0f);
		uniforms.vert.getUniforms().M = model[model_index]->m_TransformationMatrix;
		uniforms.vert.getUniforms().V = ATLAS::TranslationMatrix4f(0.0f, 0.0f, 3.0f);
		uniforms.vert.getUniforms().P = ATLAS::PerspectiveMatrix4f(
			(real32)uniforms.buffer.width / (real32)uniforms.buffer.height,
			70.0f, 0.1f, 1000.0f);

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