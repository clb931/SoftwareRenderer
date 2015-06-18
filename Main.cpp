#include "ATLAS_Stdafx.h"
#include "ATLAS_Color.cpp"
#include "ATLAS_Vector.cpp"
#include "ATLAS_Vertex.cpp"
#include "ATLAS_Matrix.cpp"
#include "ATLAS_Texture.cpp"
#include "ATLAS_Model.cpp"
#include "ATLAS_Win32.cpp"
#include "ATLAS_RenderContext.cpp"
#include <Windows.h>
#undef CreateWindow

using namespace ATLAS;

LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

GLOBAL Window window;
GLOBAL RenderContext *pRC;
GLOBAL Texture *font[2];
GLOBAL Model *model[3];
GLOBAL int32 font_index = 0;
GLOBAL int32 model_index = 0;

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
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		if (wParam == VK_F1) {
			if (model_index == 0) {
				model_index = 2;
				Matrix4f T = TranslationMatrix(0.0f, 0.0f, -4.0f);
				Matrix4f R = RotationMatrix(-90.0f, 0.0f, 0.0f);
				model[model_index]->m_TransformationMatrix = T * R;
			} else if (model_index == 1) {
				model_index--;
				Matrix4f T = TranslationMatrix(0.0f, 0.0f, -3.0f);
				Matrix4f R = RotationMatrix(0.0f, 0.0f, 0.0f);
				model[model_index]->m_TransformationMatrix = T * R;
			} else if (model_index == 2) {
				model_index--;
				Matrix4f T = TranslationMatrix(0.0f, 0.0f, -300.0f);
				Matrix4f R = RotationMatrix(-90.0f, 0.0f, 0.0f);
				model[model_index]->m_TransformationMatrix = T * R;
			}
		}
		if (wParam == VK_F2) {
			if (model_index == 0) {
				model_index++;
				Matrix4f T = TranslationMatrix(0.0f, 0.0f, -300.0f);
				Matrix4f R = RotationMatrix(-90.0f, 0.0f, 0.0f);
				model[model_index]->m_TransformationMatrix = T * R;
			} else if (model_index == 1) {
				model_index++;
				Matrix4f T = TranslationMatrix(0.0f, 0.0f, -4.0f);
				Matrix4f R = RotationMatrix(-90.0f, 0.0f, 0.0f);
				model[model_index]->m_TransformationMatrix = T * R;
			} else if (model_index == 2) {
				model_index = 0;
				Matrix4f T = TranslationMatrix(0.0f, 0.0f, -3.0f);
				Matrix4f R = RotationMatrix(0.0f, 0.0f, 0.0f);
				model[model_index]->m_TransformationMatrix = T * R;
			}
		}
		if (wParam == VK_F3) {
			AtlasEnum draw_style = (AtlasEnum)(pRC->GetDrawStyle() + 1);
			if (draw_style > DRAW_TRIANGLES)
				draw_style = DRAW_POINTS;

			pRC->SetDrawStyle(draw_style);
		}
		if (wParam == VK_F4) {
			BlendMode blend_mode = (BlendMode)(pRC->GetBlendMode() + 1);
			if (blend_mode > BLEND_LIGHTEN)
				blend_mode = BLEND_NORMAL;

			pRC->SetBlendMode(blend_mode);
		}
		if (wParam == VK_F5) {
			pRC->SetFlag(DEPTH_TEST, !pRC->GetFlag(DEPTH_TEST));
		}
		if (wParam == VK_F6) {
			pRC->SetFlag(CULL_FACES, !pRC->GetFlag(CULL_FACES));
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

void DrawString(RenderContext *render_context, const char *str, Texture *font,
	real32 size = 24.0f, Color color = Color::WHITE, real32 x = -1.0f, real32 y = 1.0f)
{
	uint32 length = strlen(str);
	if (length == 0)
		return;

	const real32 one_over_16 = 1.0f / 16.0f;
	size /= 50.0f;
	Vertex vertices[4];
	uint32 k = 0;

	bool32 old_cull_face = render_context->GetFlag(CULL_FACES);
	bool32 old_depth_test = render_context->GetFlag(DEPTH_TEST);
	AtlasEnum old_draw_style = render_context->GetDrawStyle();
	Texture	*old_texture = render_context->GetTexture();
	BlendMode old_blend_mode = render_context->GetBlendMode();
	render_context->SetFlag(CULL_FACES, false);
	render_context->SetFlag(DEPTH_TEST, false);
	render_context->SetDrawStyle(DRAW_TRIANGLES);
	render_context->SetTexture(font);
	render_context->SetBlendMode(BLEND_DARKEN);

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

	render_context->SetFlag(CULL_FACES, old_cull_face);
	render_context->SetFlag(DEPTH_TEST, old_depth_test);
	render_context->SetDrawStyle(old_draw_style);
	render_context->SetTexture(old_texture);
	render_context->SetBlendMode(old_blend_mode);
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

		render_context->SetTexture(model->m_Texture);
		render_context->DrawTriangle(v1, v2, v3);
	}
}

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, PWSTR pCmdLine, int nCmdShow)
{
	if (!window.Create(L"SoftwareRenderer", NULL))
		return -1;

	RenderContext rc(window.GetBuffer(),
		window.GetBufferWidth(), window.GetBufferHeight(),
		window.GetBufferBPP());

	pRC = &rc;
	pRC->SetClearColor(Color(0.1f, 0.2f, 0.3f));
	pRC->SetFlag(CULL_FACES, true);
	pRC->SetFlag(DEPTH_TEST, true);

	Texture font0("Consolas.bmp");
	Texture font1("font1.bmp");
	font[0] = &font0;
	font[1] = &font1;

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
		ATLAS::Polygon(2, 1, 0)		//left
	};

	Texture cubetexture("texture.bmp");
	Model cube(verts, 8, polys, 12, &cubetexture);
	cube.SetName("Cube");
	cube.m_TransformationMatrix = TranslationMatrix(0.0f, 0.0f, -3.0f);
	Texture spaceshiptexture("spaceshiptexture.bmp");
	Model spaceship("spaceship.3DS", &spaceshiptexture);
	Model monkey("monkey.3DS", &spaceshiptexture);
	model[0] = &cube;
	model[1] = &spaceship;
	model[2] = &monkey;

	char str[512] = "";
	LARGE_INTEGER current_time, last_time, elapsed_time, frequency;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&last_time);

	while (window.HandleMessages()) {
		QueryPerformanceCounter(&current_time);
		elapsed_time.QuadPart = current_time.QuadPart - last_time.QuadPart;
		elapsed_time.QuadPart *= 1000000;
		elapsed_time.QuadPart /= frequency.QuadPart;
		real64 delta_time = elapsed_time.QuadPart / 1000.0f;
		last_time = current_time;

		PERSIST real32 rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;
		rotX += (real32)delta_time / 10.0f;
		rotY += (real32)delta_time / 20.0f;
		rotZ += (real32)delta_time / 30.0f;
		if (fabs(rotX) > 360.0f)
			rotX = 0.0f;
		if (fabs(rotY) > 360.0f)
			rotY = 0.0f;
		if (fabs(rotZ) > 360.0f)
			rotZ = 0.0f;

		char *draw_style;
		AtlasEnum ds = pRC->GetDrawStyle();
		if (ds == DRAW_TRIANGLES)
			draw_style = "Triangles";
		else if (ds == DRAW_LINES)
			draw_style = "Lines";
		else if (ds == DRAW_POINTS)
			draw_style = "Points";

		Matrix4f P = PerspectiveMatrix((real32)window.GetBufferWidth() / (real32)window.GetBufferHeight(),
			70.0f, 0.1f, 1000.0f);
		Matrix4f R = RotationMatrix(rotX, rotY, rotZ);
		Matrix4f MVP = P * model[model_index]->m_TransformationMatrix * R;

		pRC->Clear(FRAME_BUFFER | DEPTH_BUFFER);
			DrawModel(pRC, model[model_index], MVP);
			sprintf_s(str, 512,
				"fps: %i dt: %.2fms\n"
				"[F1][F2]Model: %s\n"
				"[F3]Draw Style: %s\n"
				"[F4]Blend Mode: %s\n"
				"[F5]Depth Test: %s\n"
				"[F6]Cull Faces: %s\n"
				"[F7]Font: %i\n"
				"Point Size: %i\n"
				"Triangles: %i\n"
				"Pos X: %+.2f\n"
				"Pos Y: %+.2f\n"
				"Pos Z: %+.2f\n"
				"Rot X: %+.2f\n"
				"Rot Y: %+.2f\n"
				"Rot Z: %+.2f\n",
				(int)(1.0f / (delta_time / 1000.0f)), delta_time,
				model[model_index]->GetName(),
				draw_style,
				GetBlendModeName(pRC->GetBlendMode()),
				pRC->GetFlag(DEPTH_TEST) != 0 ? "true" : "false",
				pRC->GetFlag(CULL_FACES) != 0 ? "true" : "false",
				font_index, pRC->GetPointSize(),
				model[model_index]->m_NumPolygons,
				model[model_index]->m_TransformationMatrix.a2[0][3],
				model[model_index]->m_TransformationMatrix.a2[1][3],
				model[model_index]->m_TransformationMatrix.a2[2][3],
				rotX, rotY, rotZ);
		 	DrawString(pRC, str, font[font_index], 3.0f, Color::RED);
		window.Flip();
	}

	return(0);
}