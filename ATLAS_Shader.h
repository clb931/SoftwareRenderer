#ifndef ATLAS_SHADER_H
#include "ATLAS_Vector.h"
#include "ATLAS_Color.h"
#include "ATLAS_Matrix.h"
#include "ATLAS_Model.h"
#include <cassert>
#include <thread>
#include <vector>


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
        };

        struct Outputs {
            ATLAS::Vector4f pos;
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

            out.pos = ATLAS::Vector4f(in.pos, 1.0f);
            out.color = ATLAS::Color::RED;

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

                    vertices[0].pos = in.vertices[in.polygons[p].v1].pos;
                    vertices[0].color = in.vertices[in.polygons[p].v1].color;

                    vertices[1].pos = in.vertices[in.polygons[p].v2].pos;
                    vertices[1].color = in.vertices[in.polygons[p].v2].color;

                    vertices[2].pos = in.vertices[in.polygons[p].v3].pos;
                    vertices[2].color = in.vertices[in.polygons[p].v3].color;

                    for (int i = 0; i < 3; ++i) {
                        VertexShader::Inputs vertIn{};
                        vertIn.pos = ATLAS::Vector3f(vertices[i].pos.x, vertices[i].pos.y, vertices[i].pos.z);
                        VertexShader::Outputs vertOut = uniforms.vert.run(vertIn);
                        vertices[i].pos = vertOut.pos;
                        vertices[i].color = vertOut.color;
                    }

                    DrawTriangle(vertices);
#ifdef THREADING
                });
#endif
            }

            for (auto &t : threads) {
                t.join();
            }
        }

        void clear() {
            for (int i = 0; i < uniforms.buffer.width * uniforms.buffer.height; ++i) {
                ((uint32*)uniforms.buffer.data)[i] = 0;
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
            real32	x_diff = (real32)(x_max - x_min);
            if (!x_diff)
                return;

            real32	one_over_x_diff = 1.0f / x_diff;
            real32	depth_step = (right->depth - left->depth) * one_over_x_diff;
            real32	depth = left->depth + depth_step * x_prestep;
            real32	one_over_z_step = (right->one_over_z - left->one_over_z) * one_over_x_diff;
            real32	one_over_z = left->one_over_z + one_over_z_step * x_prestep;

            ATLAS::Color color_step = (right->color - left->color) * one_over_x_diff;
            ATLAS::Color color = left->color + color_step * x_prestep;

            for (uint32 x = x_min; x < x_max; ++x) {
                real32	z = 1.0f / one_over_z;

                FragmentShader::Inputs fragIn;
                fragIn.color = color * z;
                FragmentShader::Outputs fragOut = uniforms.frag.run(fragIn);
                DrawPixel(x, y, fragOut.color.toColor32());

                depth += depth_step;
                one_over_z += one_over_z_step;
                color += color_step;
            }
        }

        void ScanTriangle(Edge *shortE, Edge *longE) {
            Edge *left = shortE;
            Edge* right = longE;

            if (left->x > right->x)
                std::swap(left, right);

            if ((left->y_diff > -FLT_EPSILON && left->y_diff < FLT_EPSILON)
                    || (right->y_diff > -FLT_EPSILON && right->y_diff < FLT_EPSILON))
                return;
            
            if (shortE->y_min > uniforms.buffer.height || shortE->y_max > uniforms.buffer.height)
                return;

            for (uint32 y = shortE->y_min; y < shortE->y_max; ++y) {
                FillScanLine(left, right, y);
                right->Step();
                left->Step();
            }
        }

        void DrawTriangle(Vertex vertices[3]) {
            // screen transform
            vertices[0].pos = uniforms.screenTransform * vertices[0].pos;
            vertices[1].pos = uniforms.screenTransform * vertices[1].pos;
            vertices[2].pos = uniforms.screenTransform * vertices[2].pos;
            
            // perspective divide
            vertices[0].pos /= vertices[0].pos.w;
            vertices[1].pos /= vertices[1].pos.w;
            vertices[2].pos /= vertices[2].pos.w;

            // sort
            if (vertices[2].pos.y <  vertices[1].pos.y) {
                std::swap(vertices[2], vertices[1]);
            }
            if ( vertices[1].pos.y <  vertices[0].pos.y) {
                std::swap(vertices[1], vertices[0]);
            }
            if ( vertices[2].pos.y <  vertices[1].pos.y) {
                std::swap(vertices[2], vertices[1]);
            }

            // scan
            Edge bot2top(vertices[0], vertices[2]);
            Edge bot2mid(vertices[0], vertices[1]);
            Edge mid2top(vertices[1], vertices[2]);
            ScanTriangle(&bot2top, &bot2mid);
            ScanTriangle(&bot2top, &mid2top);
        }
    };
}

#define ATLAS_SHADER_H
#endif
