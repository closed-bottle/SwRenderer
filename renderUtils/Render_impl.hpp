#include "Render.h"
#include "RenderCmd.h"


namespace {

    template<typename T>
    const T* SampleData(const uint8_t* _data, const uint64_t& _offset, const uint64_t& _i) {
        return reinterpret_cast<const T*>(_data + _offset + (sizeof(T) * _i));
    }

    void ClipSpaceScreenSpace(Image& _render_target, Lamp::Vec4f& _v) {
        _v /= _v.w;

        const float f_width  = static_cast<float>(_render_target.Width());
        const float f_height = static_cast<float>(_render_target.Height());

        // Flipping y elements here, our window coordinates top left will be 0, 0. when it is -1, 1 in NDC.
        const Lamp::Mat4f viewport_transform = Lamp::Mat4f::Translate(f_width * .5f, f_height * .5f, 0.0f)
                                                * Lamp::Mat4f::Scale(f_width * .5f, f_height * -.5f, 1.0f);

        // TODO : How to deal with near and far plane?
        _v = viewport_transform * _v;
    }

    // Note that it is not the proper algorithm to plot points on the screen,
    // it is unstable due to the nature direct casting.
    // Only for quick demonstration.
    void DrawPointShader(const RenderCmdInfo& _cmd_info) {
        // upper left = origin.
        auto& render_target = _cmd_info.render_info_->_color_att->image_;
        const auto& uniform = static_cast<const Render::UMvp*>(_cmd_info.uniform_);

        for (uint64_t i = 0; i < _cmd_info.vertex_buffer_->count_; ++i) {
            auto v3 = *(static_cast<const Lamp::Vec3f*>(_cmd_info.vertex_buffer_->data_) + (sizeof(Lamp::Vec3f) * i));
            Lamp::Vec4f v4 = {v3.x, v3.y, v3.z, 1};

            v4 = uniform->mvp * v4;

            ClipSpaceScreenSpace(render_target, v4);

            constexpr uint8_t color[] = {255, 0, 255};
            if (v4.x > 0 && v4.x < render_target.Width() && v4.y > 0 && v4.y < render_target.Height()) {
                void* ptr = static_cast<uint8_t *>(render_target.Data())
                            + render_target.Width() * (uint32_t)v4.y + (uint32_t)v4.x;
                memcpy(ptr, color, render_target.Stride());
            }
        }
    }

    // https://en.wikipedia.org/wiki/Bresenham's_line_algorithm
    // https://zingl.github.io/bresenham.html
    // Implementation based on error increment.
    // This implementation is based on the paper released by Alois Zingl,
    // "A Rasterizing Algorithm for Drawing Curves".
    // Copyright (c) Alois Zingl
    // The code (function "plotLine") Licensed under the MIT License

    void plotLine(Image& _render_target, const Lamp::Vec4f& _start, const Lamp::Vec4f& _end) {
        constexpr uint8_t color[] = {255, 255, 0};

        int x0 = _start.x;
        int x1 = _end.x;
        int y0 = _start.y;
        int y1 = _end.y;


        int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
        int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
        int err = dx+dy, e2; /* error value e_xy */
        for (;;){ /* loop */
            if (x0 > 0 && x0 < _render_target.Width() && y0 > 0 && y0 < _render_target.Height()) {
                void* ptr = static_cast<uint8_t *>(_render_target.Data())
                            + ((_render_target.Width() * y0 + x0) * _render_target.Stride());
                memcpy(ptr, color, _render_target.Stride());
            }
            e2 = 2*err;
            if (e2 >= dy) { /* e_xy+e_x > 0 */
                if (x0 == x1) break;
                err += dy; x0 += sx;
            }
            if (e2 <= dx) { /* e_xy+e_y < 0 */
                if (y0 == y1) break;
                err += dx; y0 += sy;
            }
        }
    }

    // Assume primitive is always triangle strip.
    // It can be added to template later if needed to implement other primitives.
    void DrawTriangleLineShader(const RenderCmdInfo& _cmd_info) {
        const auto* vertices = static_cast<Lamp::Vec3f*>(_cmd_info.vertex_buffer_->data_);
        auto& render_target = _cmd_info.render_info_->_color_att->image_;
        auto& index_buffer = _cmd_info.index_buffer_;
        const auto& uniform = static_cast<const Render::UMvp*>(_cmd_info.uniform_);


        for (uint64_t i = 0; i < index_buffer->count_; i += 3) {
            auto i0 = *(static_cast<uint32_t*>(index_buffer->data_) + i);
            auto i1 = *(static_cast<uint32_t*>(index_buffer->data_) + i+1);
            auto i2 = *(static_cast<uint32_t*>(index_buffer->data_) + i+2);

            Lamp::Vec4f v0 = uniform->mvp * Lamp::Vec4f(vertices[i0].x, vertices[i0].y, vertices[i0].z, 1.0f);
            Lamp::Vec4f v1 = uniform->mvp * Lamp::Vec4f(vertices[i1].x, vertices[i1].y, vertices[i1].z, 1.0f);
            Lamp::Vec4f v2 = uniform->mvp * Lamp::Vec4f(vertices[i2].x, vertices[i2].y, vertices[i2].z, 1.0f);

            ClipSpaceScreenSpace(render_target, v0);
            ClipSpaceScreenSpace(render_target, v1);
            ClipSpaceScreenSpace(render_target, v2);

            // TODO : exclude already rendered lines.
            auto l0 = v2 - v0;
            auto l1 = v1 - v2;
            auto l2 = v0 - v1;

            plotLine(render_target, v0, v2);
            plotLine(render_target, v2, v1);
            plotLine(render_target, v1, v0);
        }
    }
}


void Render::Draw(const RenderCmdInfo& _cmd_info) {
    switch (_cmd_info.uniform_->sType) {
        case ShaderName::PointShader:
            DrawPointShader(_cmd_info);
            break;
        case ShaderName::LineShader:
            DrawTriangleLineShader(_cmd_info);
            break;
    }
}