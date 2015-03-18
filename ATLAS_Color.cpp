#include "ATLAS_Stdafx.h"

namespace ATLAS
{
	Color::Color(real32 r, real32 g, real32 b, real32 a)
	{
		R = min(1.0f, r);
		G = min(1.0f, g);
		B = min(1.0f, b);
		A = min(1.0f, a);
	}
	Color32 Color::toColor32() const
	{
		return Color32(
			((uint32)(A * 255.0f) << 24) |
			((uint32)(R * 255.0f) << 16) |
			((uint32)(G * 255.0f) << 8) |
			((uint32)(B * 255.0f) << 0));
	}
	Color Color::operator+=(const Color &top)
	{
		R += top.R;
		G += top.G;
		B += top.B;
		A += top.A;
		return *this;
	}
	Color Color::operator-(const Color &top) const
	{
		return Color(R - top.R, G - top.G, B - top.B, A - top.A);
	}
	Color Color::operator*(const Color &top) const
	{
		return Color(R * top.R, G * top.G, B * top.B, A * top.A);
	}
	Color Color::operator/(const Color &top) const
	{
		if (top.R == 0.0f && top.G == 0.0f && top.B == 0.0f)
			return Color();

		Color c(R / top.R, G / top.G, B / top.B, A / top.A);
		return c;
	}
	Color Color::operator*(real32 f) const
	{
		return Color(R * f, G * f, B * f, A * f);
	}
	Color Color::operator/(real32 f) const
	{
		return Color(R / f, G / f, B / f, A / f);
	}
	Color Color::operator~()
	{
		return Color(1.0f - R, 1.0f - G, 1.0f - B, 1.0f - A);
	}

	Color operator+(Color bottom, const Color &top)
	{
		return bottom += top;
	}

	Color BlendAdd(Color bottom, Color top)
	{
		return bottom + top;
	}
	Color BlendSubtract(Color bottom, Color top)
	{
		return bottom - top;
	}
	Color BlendMultiply(Color bottom, Color top)
	{
		return bottom * top;
	}
	Color BlendScreen(Color bottom, Color top)
	{
		return ~(~bottom * ~top);
	}
	Color BlendOverlay(Color bottom, Color top)
	{
		Color c1 = (bottom * top) * 2.0f;
		Color c2 = ~((~bottom * ~top) * 2.0f);
		return Color(bottom.R < 0.5f ? c1.R : c2.R,
			bottom.G < 0.5f ? c1.G : c2.G,
			bottom.B < 0.5f ? c1.B : c2.B,
			bottom.A < 0.5f ? c1.A : c2.A);
	}
	Color BlendDodge(Color bottom, Color top)
	{
		return bottom / ~top;
	}
	Color BlendBurn(Color bottom, Color top)
	{
		return ~((~bottom) / top);
	}
	Color BlendDifference(Color bottom, Color top)
	{
		Color c;
		c.R = bottom.R < top.R ? top.R - bottom.R : bottom.R - top.R;
		c.G = bottom.G < top.G ? top.G - bottom.G : bottom.G - top.G;
		c.B = bottom.B < top.B ? top.B - bottom.B : bottom.B - top.B;
		c.A = bottom.A < top.A ? top.A - bottom.A : bottom.A - top.A;
		return c;
	}
	Color BlendDarken(Color bottom, Color top)
	{
		return Color(min(bottom.R, top.R), min(bottom.G, top.G), min(bottom.B, top.B), min(bottom.A, top.A));
	}
	Color BlendLighten(Color bottom, Color top)
	{
		return Color(max(bottom.R, top.R), max(bottom.G, top.G), max(bottom.B, top.B), max(bottom.A, top.A));
	}

	
	struct hsv
	{
		real32 h;
		real32 s;
		real32 v;
	};

	hsv rgb2hsv(Color in)
	{
		hsv out;
		real32 min, max, delta;

		min = in.R < in.G ? in.R : in.G;
		min = min < in.B ? min : in.B;

		max = in.R > in.G ? in.R : in.G;
		max = min > in.B ? min : in.B;

		out.v = max;
		delta = max - min;
		if (max > 0.0) {
			out.s = (delta / max);
		}
		else {
			out.s = 0.0;
			out.h = NAN;
			return out;
		}
		if (in.R >= max)
			out.h = (in.G - in.B) / delta;
		else if (in.G >= max)
			out.h = 2.0f + (in.B - in.R) / delta;
		else
			out.h = 4.0f + (in.R - in.G) / delta;

		out.h *= 60.0f;

		if (out.h < 0.0)
			out.h += 360.0f;

		return out;
	}
	Color hsv2rgb(hsv in)
	{
		Color out;
		real32 hh, p, q, t, ff;
		int32 i;

		if (in.s <= 0.0f) {  
			out.R = in.v;
			out.G = in.v;
			out.B = in.v;
			return out;
		}

		hh = in.h;
		if (hh >= 360.0f) hh = 0.0f;
		hh /= 60.0f;
		i = (long)hh;
		ff = hh - i;
		p = in.v * (1.0f - in.s);
		q = in.v * (1.0f - (in.s * ff));
		t = in.v * (1.0f - (in.s * (1.0f - ff)));

		switch (i) {
		case 0:
			out.R = in.v;
			out.G = t;
			out.B = p;
			break;
		case 1:
			out.R = q;
			out.G = in.v;
			out.B = p;
			break;
		case 2:
			out.R = p;
			out.G = in.v;
			out.B = t;
			break;

		case 3:
			out.R = p;
			out.G = q;
			out.B = in.v;
			break;
		case 4:
			out.R = t;
			out.G = p;
			out.B = in.v;
			break;
		case 5:
		default:
			out.R = in.v;
			out.G = p;
			out.B = q;
			break;
		}
		return out;
	}

	Color LerpColor(Color min, Color max, real32 step)
	{
		//hsv ca = rgb2hsv(min);
		//hsv cb = rgb2hsv(min);
		//hsv result;

		//result.h = ca.h * (1 - step) + cb.h * step;
		//result.s = ca.s * (1 - step) + cb.s * step;
		//result.v = ca.v * (1 - step) + cb.v * step;

		//return hsv2rgb(result);

		return (min * (1 - step) + max * step);
	}

	const Color Color::RED = Color(1.0f, 0.0f, 0.0f, 0.1f);
	const Color Color::GREEN = Color(0.0f, 1.0f, 0.0f, 0.1f);
	const Color Color::BLUE = Color(0.0f, 0.0f, 1.0f, 0.1f);
	const Color Color::BLACK = Color(0.0f, 0.0f, 0.0f, 0.1f);
	const Color Color::CYAN = Color(0.0f, 1.0f, 1.0f, 0.1f);
	const Color Color::MAGENTA = Color(1.0f, 0.0f, 1.0f, 0.1f);
	const Color Color::YELLOW = Color(1.0f, 1.0f, 0.0f, 0.1f);
	const Color Color::WHITE = Color(1.0f, 1.0f, 1.0f, 0.1f);
}