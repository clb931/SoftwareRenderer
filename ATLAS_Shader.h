#ifndef ATLAS_SHADER_H
#include "ATLAS_Vector.h"
#include "ATLAS_Color.h"
#include "ATLAS_Matrix.h"
#include "ATLAS_Model.h"
#include <cassert>
#include <thread>
#include <vector>


static bool drawTris = true;
static bool drawLines = true;
static bool drawPoints = true;

namespace ATL {
    struct Vertex {
        ATLAS::Vector4f pos;
        ATLAS::Color    color;
    };

    struct Edge {
		int32	y_min, y_max;
		real32	y_diff;
		real32	x_step, x;
		real32	depth_step, depth;
		real32	one_over_z_step, one_over_z;
		ATLAS::Color	color_step, color;
		ATLAS::UV		uv_step, uv;

        Edge(Vertex bot, Vertex top) {
            y_min = (int32)ceil(bot.pos.y);
            y_max = (int32)ceil(top.pos.y);

            real32 y_prestep = y_min - bot.pos.y;

            y_diff = top.pos.y - bot.pos.y;
            real32	x_diff = top.pos.x - bot.pos.x;

            x_step = x_diff / y_diff;
            x = bot.pos.x + x_step * y_prestep;
            real32 x_prestep = x - bot.pos.x;

            depth_step = (top.pos.z - bot.pos.z) / y_diff;
            depth = bot.pos.z + depth_step * y_prestep;

            one_over_z_step = ((1.0f / top.pos.w) - (1.0f / bot.pos.w)) / y_diff;
            one_over_z = (1.0f / bot.pos.w) + one_over_z_step * y_prestep;

            color_step = (top.color - bot.color) / y_diff;
            color = bot.color + color_step * y_prestep;

            // uv_step = (top.uv - bot.uv) / y_diff;
            // uv = bot.uv + uv_step * y_prestep;
        }

        void Step() {
            x += x_step;
            depth += depth_step;
            color += color_step;
            uv += uv_step;
            one_over_z += one_over_z_step;
        }
    };

    class VertexShader {
    public:
        struct Inputs {
            ATLAS::Vector3f pos;
            ATLAS::Color color;
        };

        struct Outputs {
            ATLAS::Vector4f pos;
            ATLAS::Color color;
        };

        struct Uniforms {
            ATLAS::Matrix4f M = ATLAS::IdentityMatrix4f;
            ATLAS::Matrix4f V = ATLAS::IdentityMatrix4f;
            ATLAS::Matrix4f P = ATLAS::IdentityMatrix4f;
        };

        Uniforms& getUniforms() {
            return uniforms;
        }

        void setUniforms(Uniforms uniforms) {
            this->uniforms = uniforms;
        }

        Outputs run(Inputs in) {
            Outputs out{};

            out.pos = ATLAS::Vector4f(in.pos, 1.0f);
            ATLAS::Matrix4f MVP = uniforms.P * uniforms.V * uniforms.M;
            out.pos *= MVP;
            out.color = in.color;

            return out;
        }

    private:
        Uniforms uniforms;
    };

    class FragmentShader {
    public:
        struct Inputs {
            ATLAS::Color color;
        };

        struct Outputs {
            ATLAS::Color color;
        };

        struct Uniforms {
            
        };

        Uniforms& getUniforms() {
            return uniforms;
        }

        void setUniforms(Uniforms uniforms) {
            this->uniforms = uniforms;
        }

        Outputs run(Inputs in) {
            Outputs out{};

            out.color = in.color;

            return out;
        }

    private:
        Uniforms uniforms;
    };

    class Rasterizer {
    public:
        struct Inputs {
            uint32 polygonCount;
            ATLAS::Polygon* polygons;
            uint32 vertexCount;
            ATLAS::Vertex* vertices;
        };

        struct Buffer {
            uint32 width, height, bpp;
            void* data;
        };

        struct Uniforms {
            Buffer buffer;
            ATLAS::Matrix4f screenTransform;
            VertexShader vert;
            FragmentShader frag;
        };

        Uniforms& getUniforms() {
            return uniforms;
        }

        void setUniforms(Uniforms uniforms) {
            this->uniforms = uniforms;
        }

        void run(Inputs in) {
            assert(uniforms.buffer.bpp == 32);

            std::vector<std::thread> threads;
            for (uint32 p = 0; p < in.polygonCount; ++p) {
#ifdef THREADING
                threads.emplace_back([this, &in, p]() {
#endif
                    Vertex vertices[3];
                    ATLAS::Vector3f pos[3];
                    ATLAS::Color col[3];

                    pos[0] = in.vertices[in.polygons[p].v1].pos;
                    col[0] = in.vertices[in.polygons[p].v1].color;

                    pos[1] = in.vertices[in.polygons[p].v2].pos;
                    col[1] = in.vertices[in.polygons[p].v2].color;

                    pos[2] = in.vertices[in.polygons[p].v3].pos;
                    col[2] = in.vertices[in.polygons[p].v3].color;

                    for (int i = 0; i < 3; ++i) {
                        VertexShader::Inputs vertIn{};
                        vertIn.pos = pos[i];
                        vertIn.color = col[i];
                        VertexShader::Outputs vertOut = uniforms.vert.run(vertIn);
                        vertices[i].pos = vertOut.pos;
                        vertices[i].color = vertOut.color;
                    }

                    if (drawTris) {
                        DrawTriangle(vertices[0], vertices[1], vertices[2]);
                    }

                    if (drawLines) {
                        DrawLine(vertices[0], vertices[1]);
                        DrawLine(vertices[1], vertices[2]);
                        DrawLine(vertices[2], vertices[0]);
                    }
                    
                    if (drawPoints) {
                        DrawPoint(vertices[0]);
                        DrawPoint(vertices[1]);
                        DrawPoint(vertices[2]);
                    }
#ifdef THREADING
                });
#endif
            }

            for (auto &t : threads) {
                t.join();
            }
        }

        void clear() {
            uint32 color = ATLAS::Color(0.1f, 0.2f, 0.3f, 1.0f).toColor32();
            for (int i = 0; i < uniforms.buffer.width * uniforms.buffer.height; ++i) {
                ((uint32*)uniforms.buffer.data)[i] = color;
            }
        }

    private:
        Uniforms uniforms;

        void DrawPixel(uint32 x, uint32 y, uint32 color) {
            uint32* pixel = (uint32*)uniforms.buffer.data + (y * uniforms.buffer.width) + x;
            *pixel = color;
        }

        void FillScanLine(Edge *left, Edge *right, uint32 y) {
            uint32 x_min = (uint32)ceil(left->x);
            uint32 x_max = (uint32)ceil(right->x);

            if (x_min > uniforms.buffer.width || x_max > uniforms.buffer.width)
                return;

            real32 x_prestep = x_min - left->x;
            real32 x_diff = (real32)(x_max - x_min);
            // if (!x_diff)
            if (x_diff > -FLT_EPSILON && x_diff < FLT_EPSILON)
                return;

            real32	one_over_x_diff = 1.0f / x_diff;
            real32	depth_step = (right->depth - left->depth) * one_over_x_diff;
            real32	depth = left->depth + depth_step * x_prestep;
            real32	one_over_z_step = (right->one_over_z - left->one_over_z) * one_over_x_diff;
            real32	one_over_z = left->one_over_z + one_over_z_step * x_prestep;

            ATLAS::Color color_step = (right->color - left->color) * one_over_x_diff;
            ATLAS::Color color = left->color + color_step * x_prestep;

            // UV		uv_step = (pRight->uv - pLeft->uv) * one_over_x_diff;
            // UV		uv = pLeft->uv + uv_step * x_prestep;
            // real32	*pDepthBuffer = (real32 *)m_DepthBuffer + y * m_Width;

            for (uint32 x = x_min; x < x_max; ++x) {
                real32	z = 1.0f / one_over_z;

                FragmentShader::Inputs fragIn;
                fragIn.color = color * z;
                FragmentShader::Outputs fragOut = uniforms.frag.run(fragIn);
                DrawPixel(x, y, fragOut.color.toColor32());

                depth += depth_step;
                one_over_z += one_over_z_step;
                color += color_step;
                // uv += uv_step;
            }
        }

        void ScanTriangle(Edge *longE, Edge *shortE) {
            Edge *left, *right;

            left = shortE->x < longE->x ? shortE : longE;
            right = shortE->x > longE->x ? shortE : longE;

            // if (!left->y_diff || !right->y_diff)
            if ((left->y_diff > -FLT_EPSILON && left->y_diff < FLT_EPSILON)
                    || (right->y_diff > -FLT_EPSILON && right->y_diff < FLT_EPSILON))
                return;
            
            uint32 y_min = (uint32)ceil(shortE->y_min);
            uint32 y_max = (uint32)ceil(shortE->y_max);
            if (y_min > uniforms.buffer.height || y_max > uniforms.buffer.height)
                return;

            for (uint32 y = y_min; y < y_max; ++y) {
                FillScanLine(left, right, y);
                right->Step();
                left->Step();
            }
        }

        void DrawTriangle(Vertex v1, Vertex v2, Vertex v3) {
            // screen transform & perspective divide
            v1.pos *= uniforms.screenTransform;
            v2.pos *= uniforms.screenTransform;
            v3.pos *= uniforms.screenTransform;

            v1.pos.x /= v1.pos.w;
            v1.pos.y /= v1.pos.w;
            v1.pos.z /= v1.pos.w;

            v2.pos.x /= v2.pos.w;
            v2.pos.y /= v2.pos.w;
            v2.pos.z /= v2.pos.w;

            v3.pos.x /= v3.pos.w;
            v3.pos.y /= v3.pos.w;
            v3.pos.z /= v3.pos.w;


            // sort
            if (v3.pos.y < v2.pos.y)
                std::swap(v3, v2);
            if (v2.pos.y < v1.pos.y)
                std::swap(v2, v1);
            if (v3.pos.y < v2.pos.y)
                std::swap(v3, v2);

            // scan
            Edge bot2top(v1, v3);
            Edge bot2mid(v1, v2);
            Edge mid2top(v2, v3);
            ScanTriangle(&bot2top, &bot2mid);
            ScanTriangle(&bot2top, &mid2top);
        }

        void DrawLine(Vertex v1, Vertex v2)
        {
            v1.pos = (uniforms.screenTransform * v1.pos) / v1.pos.w;
            v2.pos = (uniforms.screenTransform * v2.pos) / v2.pos.w;

            real32 xdiff = (v2.pos.x - v1.pos.x);
            real32 ydiff = (v2.pos.y - v1.pos.y);

            if (xdiff == 0.0f && ydiff == 0.0f) {
                DrawPixel((uint32)v1.pos.x, (uint32)v1.pos.y, ATLAS::Color::BLACK.toColor32());
                return;
            }

            if (fabs(xdiff) > fabs(ydiff)) {
                real32 xmin, xmax;

                if (v1.pos.x < v2.pos.x) {
                    xmin = v1.pos.x;
                    xmax = v2.pos.x;
                }
                else {
                    xmin = v2.pos.x;
                    xmax = v1.pos.x;
                }

                real32 slope = ydiff / xdiff;
                for (real32 x = xmin; x <= xmax; x += 1.0f) {
                    real32 y = v1.pos.y + ((x - v1.pos.x) * slope);
                    ATLAS::Color color = v1.color + ((v2.color - v1.color) * ((x - v1.pos.x) / xdiff));
                    // real32 u = v1.uv.u + ((v2.uv.u - v1.uv.u) * ((x - v1.pos.x) / xdiff));
                    // real32 v = v1.uv.v + ((v2.uv.v - v1.uv.v) * ((x - v1.pos.x) / xdiff));
                    DrawPixel((uint32)x, (uint32)y, ATLAS::Color::BLACK.toColor32());
                }
            }
            else {
                real32 ymin, ymax;

                if (v1.pos.y < v2.pos.y) {
                    ymin = v1.pos.y;
                    ymax = v2.pos.y;
                }
                else {
                    ymin = v2.pos.y;
                    ymax = v1.pos.y;
                }

                real32 slope = xdiff / ydiff;
                for (real32 y = ymin; y <= ymax; y += 1.0f) {
                    real32 x = v1.pos.x + ((y - v1.pos.y) * slope);
                    ATLAS::Color color = v1.color + ((v2.color - v1.color) * ((y - v1.pos.y) / ydiff));
                    // real32 u = v1.uv.u + ((v2.uv.u - v1.uv.u) * ((y - v1.pos.y) / ydiff));
                    // real32 v = v1.uv.v + ((v2.uv.v - v1.uv.v) * ((y - v1.pos.y) / ydiff));
                    // Color texel = GetTexel(u, v);
                    DrawPixel((uint32)x, (uint32)y, ATLAS::Color::BLACK.toColor32());
                }
            }
        }

        void DrawPoint(Vertex v)
        {
            v.pos = (uniforms.screenTransform * v.pos) / v.pos.w;

            for (int32 y = -4; y < 4; ++y) {
                for (int32 x = -4; x < 4; ++x) {
                    // Color texel = GetTexel(v.uv.u, v.uv.v);
                    DrawPixel((uint32)v.pos.x + x, (uint32)v.pos.y + y, ATLAS::Color::BLACK.toColor32());
                }
            }
        }
    };
}

#define ATLAS_SHADER_H
#endif
