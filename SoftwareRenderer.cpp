#include "stdafx.h"
#include "ATLAS_Model.h"
#include "ATLAS_RenderContext.h"
#include "ATLAS_Texture.h"
#include "ATLAS_Win32.h"


INTERN ATLAS::Window window;
INTERN ATLAS::RenderContext *pRC;
INTERN ATLAS::Texture *font[2];
INTERN ATLAS::Model *model[3];
INTERN int32 font_index = 0;
INTERN int32 model_index = 0;

LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	switch (msg) {
		case WM_SIZE: {
			if (pRC)
				pRC->Resize(LOWORD(lParam), HIWORD(lParam));
			window.Resize(LOWORD(lParam), HIWORD(lParam));
			window.Flip();
		} break;

		case WM_KEYDOWN: {
			if (wParam == VK_ESCAPE) {
				PostQuitMessage(0);
			}
			if (wParam == VK_F1) {
				if (model_index == 0) {
					model_index = 2;
					ATLAS::Matrix4f T = ATLAS::TranslationMatrix(0.0f, 0.0f, -4.0f);
					ATLAS::Matrix4f R = ATLAS::RotationMatrix(-90.0f, 0.0f, 0.0f);
					model[model_index]->m_TransformationMatrix = T * R;
				}
				else if (model_index == 1) {
					model_index--;
					ATLAS::Matrix4f T = ATLAS::TranslationMatrix(0.0f, 0.0f, -3.0f);
					ATLAS::Matrix4f R = ATLAS::RotationMatrix(0.0f, 0.0f, 0.0f);
					model[model_index]->m_TransformationMatrix = T * R;
				}
				else if (model_index == 2) {
					model_index--;
					ATLAS::Matrix4f T = ATLAS::TranslationMatrix(0.0f, 0.0f, -300.0f);
					ATLAS::Matrix4f R = ATLAS::RotationMatrix(-90.0f, 0.0f, 0.0f);
					model[model_index]->m_TransformationMatrix = T * R;
				}
			}
			if (wParam == VK_F2) {
				if (model_index == 0) {
					model_index++;
					ATLAS::Matrix4f T = ATLAS::TranslationMatrix(0.0f, 0.0f, -300.0f);
					ATLAS::Matrix4f R = ATLAS::RotationMatrix(-90.0f, 0.0f, 0.0f);
					model[model_index]->m_TransformationMatrix = T * R;
				}
				else if (model_index == 1) {
					model_index++;
					ATLAS::Matrix4f T = ATLAS::TranslationMatrix(0.0f, 0.0f, -4.0f);
					ATLAS::Matrix4f R = ATLAS::RotationMatrix(-90.0f, 0.0f, 0.0f);
					model[model_index]->m_TransformationMatrix = T * R;
				}
				else if (model_index == 2) {
					model_index = 0;
					ATLAS::Matrix4f T = ATLAS::TranslationMatrix(0.0f, 0.0f, -3.0f);
					ATLAS::Matrix4f R = ATLAS::RotationMatrix(0.0f, 0.0f, 0.0f);
					model[model_index]->m_TransformationMatrix = T * R;
				}
			}
			if (wParam == VK_F3) {
				ATLAS::AtlasEnum draw_style = (ATLAS::AtlasEnum)(pRC->GetDrawStyle() + 1);
				if (draw_style > ATLAS::DRAW_TRIANGLES)
					draw_style = ATLAS::DRAW_POINTS;

				pRC->SetDrawStyle(draw_style);
			}
			if (wParam == VK_F4) {
				ATLAS::BlendMode blend_mode = (ATLAS::BlendMode)(pRC->GetBlendMode() + 1);
				if (blend_mode > ATLAS::BLEND_LIGHTEN)
					blend_mode = ATLAS::BLEND_NORMAL;

				pRC->SetBlendMode(blend_mode);
			}
			if (wParam == VK_F5) {
				pRC->SetFlag(ATLAS::DEPTH_TEST, !pRC->GetFlag(ATLAS::DEPTH_TEST));
			}
			if (wParam == VK_F6) {
				pRC->SetFlag(ATLAS::CULL_FACES, !pRC->GetFlag(ATLAS::CULL_FACES));
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

void DrawString(ATLAS::RenderContext *render_context, const char *str, ATLAS::Texture *font,
	real32 size = 24.0f, ATLAS::Color color = ATLAS::Color::WHITE, real32 x = -1.0f, real32 y = 1.0f)
{
	uint32 length = strlen(str);
	if (length == 0)
		return;

	const real32 one_over_16 = 1.0f / 16.0f;
	size /= 50.0f;
	ATLAS::Vertex vertices[4];
	uint32 k = 0;

	bool32 old_cull_face = render_context->GetFlag(ATLAS::CULL_FACES);
	bool32 old_depth_test = render_context->GetFlag(ATLAS::DEPTH_TEST);
	ATLAS::AtlasEnum old_draw_style = render_context->GetDrawStyle();
	ATLAS::Texture	*old_texture = render_context->GetTexture();
	ATLAS::BlendMode old_blend_mode = render_context->GetBlendMode();
	render_context->SetFlag(ATLAS::CULL_FACES, false);
	render_context->SetFlag(ATLAS::DEPTH_TEST, false);
	render_context->SetDrawStyle(ATLAS::DRAW_TRIANGLES);
	render_context->SetTexture(font);
	render_context->SetBlendMode(ATLAS::BLEND_DARKEN);

	ATLAS::Matrix4f P = ATLAS::OrthograohicMatrix(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 1000.0f);

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
		vertices[j].pos.x = x + (size * 0.65f) * k;
		vertices[j].pos.y = y - size;
		vertices[j].uv.u = xPos;
		vertices[j].uv.v = 1.0f - yPos - one_over_16;
		vertices[j].color = color;
		j++;

		if (str[i] == '\t')
			k += 4;
		// bottom - right
		vertices[j].pos.x = x + (size * 0.65f) * (k + 1);
		vertices[j].pos.y = y - size;
		vertices[j].uv.u = xPos + one_over_16;
		vertices[j].uv.v = 1.0f - yPos - one_over_16;
		vertices[j].color = color;
		j++;

		if (str[i] == '\t')
			k -= 4;
		// top - left
		vertices[j].pos.x = x + (size * 0.65f) * k;
		vertices[j].pos.y = y;
		vertices[j].uv.u = xPos;
		vertices[j].uv.v = 1.0f - yPos - 0.001f;
		vertices[j].color = color;
		j++;

		if (str[i] == '\t')
			k += 4;
		// top - right
		vertices[j].pos.x = x + (size * 0.65f) * (k + 1);
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

	render_context->SetFlag(ATLAS::CULL_FACES, old_cull_face);
	render_context->SetFlag(ATLAS::DEPTH_TEST, old_depth_test);
	render_context->SetDrawStyle(old_draw_style);
	render_context->SetTexture(old_texture);
	render_context->SetBlendMode(old_blend_mode);
}

void DrawModel(ATLAS::RenderContext *render_context, ATLAS::Model *model, ATLAS::Matrix4f MVP)
{
	for (uint32 p = 0; p < model->m_NumPolygons; ++p) {
		ATLAS::Vertex v1 = model->m_Vertices[model->m_Polygons[p].v1];
		ATLAS::Vertex v2 = model->m_Vertices[model->m_Polygons[p].v2];
		ATLAS::Vertex v3 = model->m_Vertices[model->m_Polygons[p].v3];

		v1.pos *= MVP;
		v2.pos *= MVP;
		v3.pos *= MVP;

		render_context->SetTexture(model->m_Texture);
		render_context->DrawTriangle(v1, v2, v3);
	}
}

LARGE_INTEGER freq;

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

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow)
{
	if (!window.Create(L"SoftwareRenderer", NULL))
		return -1;

	ATLAS::RenderContext rc(window.GetBuffer(),
		window.GetBufferWidth(), window.GetBufferHeight(),
		window.GetBufferBPP());

	pRC = &rc;
	pRC->SetClearColor(ATLAS::Color(0.1f, 0.2f, 0.3f));
	pRC->SetFlag(ATLAS::CULL_FACES, true);
	pRC->SetFlag(ATLAS::DEPTH_TEST, true);

	ATLAS::Texture font0("Consolas.bmp");
	ATLAS::Texture font1("font1.bmp");
	font[0] = &font0;
	font[1] = &font1;

	ATLAS::Vertex verts[]
	{
		ATLAS::Vertex(ATLAS::Vector4f(-0.5f, -0.5f, 0.5f),	ATLAS::UV(0.0f, 0.0f), ATLAS::Color::YELLOW),	// left  - bottom	- back
		ATLAS::Vertex(ATLAS::Vector4f(0.5f, -0.5f, 0.5f),	ATLAS::UV(1.0f, 0.0f), ATLAS::Color::WHITE),	// right - bottom	- back
		ATLAS::Vertex(ATLAS::Vector4f(0.5f, -0.5f, -0.5f),	ATLAS::UV(1.0f, 0.0f), ATLAS::Color::CYAN),		// left  - bottom	- front
		ATLAS::Vertex(ATLAS::Vector4f(-0.5f, -0.5f, -0.5f),	ATLAS::UV(0.0f, 0.0f), ATLAS::Color::GREEN),	// right - bottom	- front
		ATLAS::Vertex(ATLAS::Vector4f(-0.5f, 0.5f, 0.5f),	ATLAS::UV(0.0f, 1.0f), ATLAS::Color::RED),		// left  - top		- back
		ATLAS::Vertex(ATLAS::Vector4f(0.5f, 0.5f, 0.5f),	ATLAS::UV(1.0f, 1.0f), ATLAS::Color::MAGENTA),	// right - top		- back
		ATLAS::Vertex(ATLAS::Vector4f(0.5f, 0.5f, -0.5f),	ATLAS::UV(1.0f, 1.0f), ATLAS::Color::BLUE), 	// left  - top		- front
		ATLAS::Vertex(ATLAS::Vector4f(-0.5f, 0.5f, -0.5f),	ATLAS::UV(0.0f, 1.0f), ATLAS::Color::BLACK)		// right - top		- front
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
	ATLAS::Model cube(verts, 8, polys, 12, &cubetexture);
	cube.SetName("Cube");
	cube.m_TransformationMatrix = ATLAS::TranslationMatrix(0.0f, 0.0f, -3.0f);
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
			
			rotZ += dt * 30.0f;
			if (fabs(rotZ) > 360.0f)
				rotZ = 0.0f;

			frame_accumulator -= dt;
		}

		char *draw_style = "";
		ATLAS::AtlasEnum ds = pRC->GetDrawStyle();
		if (ds == ATLAS::DRAW_TRIANGLES)
			draw_style = "Triangles";
		else if (ds == ATLAS::DRAW_LINES)
			draw_style = "Lines";
		else if (ds == ATLAS::DRAW_POINTS)
			draw_style = "Points";
		else
			draw_style = "ERROR";

		ATLAS::Matrix4f P = ATLAS::PerspectiveMatrix((real32)window.GetBufferWidth() / (real32)window.GetBufferHeight(),
			70.0f, 0.1f, 1000.0f);
		ATLAS::Matrix4f R = ATLAS::RotationMatrix(rotX, rotY, rotZ);
		ATLAS::Matrix4f MVP = P * model[model_index]->m_TransformationMatrix * R;

		pRC->Clear(ATLAS::FRAME_BUFFER | ATLAS::DEPTH_BUFFER);
		DrawModel(pRC, model[model_index], MVP);

		PERSIST double frame_time = 0;
		frame_time = lerp(delta_time, frame_time, 0.95);
		sprintf_s(str, 512,
			"fps: %i dt: %.2f ms\n"
			"[F1][F2]Model:  %s\n"
			"[F3]Draw Style: %s\n"
			"[F4]Blend Mode: %s\n"
			"[F5]Depth Test: %s\n"
			"[F6]Cull Faces: %s\n"
			"[F7]Font:   %i\n"
			"Point Size: %i\n"
			"Triangles:  %i\n"
			"Pos X: %+.2f\n"
			"Pos Y: %+.2f\n"
			"Pos Z: %+.2f\n"
			"Rot X: %+.2f\n"
			"Rot Y: %+.2f\n"
			"Rot Z: %+.2f\n",
			(int)(1.0 / (frame_time / 1000.0)), frame_time,
			model[model_index]->GetName(),
			draw_style,
			GetBlendModeName(pRC->GetBlendMode()),
			pRC->GetFlag(ATLAS::DEPTH_TEST) != 0 ? "true" : "false",
			pRC->GetFlag(ATLAS::CULL_FACES) != 0 ? "true" : "false",
			font_index, pRC->GetPointSize(),
			model[model_index]->m_NumPolygons,
			model[model_index]->m_TransformationMatrix.a2[0][3],
			model[model_index]->m_TransformationMatrix.a2[1][3],
			model[model_index]->m_TransformationMatrix.a2[2][3],
			rotX, rotY, rotZ);
		DrawString(pRC, str, font[font_index], 3.0f, ATLAS::Color::RED);
		window.Flip();
	}

	return(0);
}