#include <thread>

#include "Pipeline.h"
#include "Render.h"
#include "RenderCmd.h"


namespace RenderImpl{
#ifdef USE_SIMD
    inline void MatrixVectorMul(const __m128& _c0, const __m128& _c1, const __m128& _c2, const __m128& _c3,
                                const float* _x, const float* _y, const float* _z, const float* _w,
                                float* _out) {
        const __m128 vv0 = _mm_load_ps(_x);
        const __m128 vv1 = _mm_load_ps(_y);
        const __m128 vv2 = _mm_load_ps(_z);
        const __m128 vv3 = _mm_load_ps(_w);

        __m128 result = _mm_set_ps(0,0,0,0);

        result = _mm_fmadd_ps(_c0, vv0, result);
        result = _mm_fmadd_ps(_c1, vv1, result);
        result = _mm_fmadd_ps(_c2, vv2, result);
        result = _mm_fmadd_ps(_c3, vv3, result);

        _mm_store_ps(_out, result);
    }
#endif
    template<typename T>
    const T* SampleData(const uint8_t* _data, const uint64_t& _offset, const uint64_t& _i) {
        return reinterpret_cast<const T*>(_data + _offset + (sizeof(T) * _i));
    }

    inline void ClipToNdc(Lamp::Vec4f &_v) {
        _v /= _v.w;
    }

    inline void NdcToWindow(const Lamp::Mat4f &_viewport, Lamp::Vec4f &_v) {
        _v = _viewport * _v;
    }

    inline Lamp::Mat4f ViewportTransform(const Viewport& _viewport) {
        const float f_width  = _viewport.width;
        const float f_height = _viewport.height;

        const Lamp::Mat4f viewport_transform
            = Lamp::Mat4f::Translate(_viewport.x + f_width * .5f,
                                     _viewport.y + f_height * .5f, 0)
            * Lamp::Mat4f::Scale(f_width * .5f, f_height * -.5f, 1);

        return viewport_transform;
    }

    template<typename T>
    T EdgeFunc(const Lamp::Vec4f& _v0, const Lamp::Vec4f& _v1, const Lamp::Vec4f& _v2) {
        const T a = static_cast<T>(_v2.x) - _v0.x;
        const T d = static_cast<T>(_v1.y) - _v0.y;

        const T b = static_cast<T>(_v2.y) - _v0.y;
        const T c = static_cast<T>(_v1.x) - _v0.x;

        // ad - bc.
        return a * d - b * c;
    }

    inline bool BackfaceCullCCW(const double& _cross) {
        return _cross > 0.0;
    }

    inline bool BackfaceCullCW(const double& _cross) {
        return _cross <= 0.0;
    }

    inline void LoadOp(const AttInfo* const _att, const Viewport* _viewport,
                       const uint32_t& _width, const uint32_t& _height) {
        auto& target = _att->image_;

        switch (_att->load_op_) {
            case LoadOp::LOAD_OP_CLEAR:
                const int x = static_cast<int>(_viewport->x);
                const int y = static_cast<int>(_viewport->y);
                const auto& clear_color = _att->clear_val_;

                for (int i = 0; i < _height; ++i) {
                    for (int j = 0; j < _width; ++j) {
                        void* color_ptr = target.Data()
                                    + (target.Width() * static_cast<uint32_t>(y + i) + static_cast<uint32_t>(x + j))
                                    * target.Stride();

                        memcpy(color_ptr, clear_color, target.Stride());
                    }
                }

                break;
        }
    }

    // Note that it is not the proper algorithm to plot points on the screen,
    // it is unstable due to the nature direct casting.
    // Only for quick demonstration.
    void DrawPointShader(const RenderCmdInfo& _cmd_info) {
        // upper left = origin.
        auto& render_target = _cmd_info.render_info_->_color_att->image_;
        const auto& uniform = static_cast<const Render::UMvp*>(_cmd_info.uniform_);

        auto& view_port = _cmd_info.view_port_;
        const uint32_t uiwidth = view_port->width;
        const uint32_t uiheight = view_port->height;
        Lamp::Mat4f viewport_transform = ViewportTransform(*view_port);

        for (uint64_t i = 0; i < _cmd_info.vertex_buffer_->count_; ++i) {
            auto v3 = *(reinterpret_cast<const Lamp::Vec3f*>(_cmd_info.vertex_buffer_->Data()) + (sizeof(Lamp::Vec3f) * i));
            Lamp::Vec4f v4 = {v3.x, v3.y, v3.z, 1};

            v4 = uniform->mvp * v4;
            ClipToNdc(v4);
            NdcToWindow(viewport_transform, v4);


            constexpr uint8_t color[] = {255, 0, 255};
            if (v4.x >= view_port->x
             && v4.x < view_port->x + uiwidth
             && v4.y >= view_port->y
             && v4.y < view_port->y + uiheight) {
                void* ptr = static_cast<uint8_t *>(render_target.Data())
                            + uiwidth * (uint32_t)v4.y + (uint32_t)v4.x;
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

    inline void plotLine(const RenderCmdInfo& _cmd_info, const Lamp::Vec4f& _start, const Lamp::Vec4f& _end) {
        constexpr uint8_t color[] = {255, 255, 0};
        auto& render_target = _cmd_info.render_info_->_color_att->image_;

        auto& view_port = _cmd_info.view_port_;
        const int x = view_port->x;
        const int y = view_port->y;
        const uint32_t uiwidth = view_port->width;
        const uint32_t uiheight = view_port->height;

        int x0 = _start.x;
        int x1 = _end.x;
        int y0 = _start.y;
        int y1 = _end.y;


        int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
        int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
        int err = dx+dy, e2; /* error value e_xy */
        for (;;){ /* loop */
            if (x0 >= x && x0 < (x + uiwidth) && y0 >= y && y0 < (y + uiheight)) {
                void* ptr = static_cast<uint8_t *>(render_target.Data())
                            + ((render_target.Width() * y0 + x0) * render_target.Stride());
                memcpy(ptr, color, render_target.Stride());
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
        auto& render_target = _cmd_info.render_info_->_color_att->image_;
        auto& vertex_buffer = _cmd_info.vertex_buffer_;
        auto& index_buffer = _cmd_info.index_buffer_;
        const auto& uniform = static_cast<const Render::UMvp*>(_cmd_info.uniform_);

        auto& view_port = _cmd_info.view_port_;
        Lamp::Mat4f viewport_transform = ViewportTransform(*view_port);

#ifdef USE_SIMD
        // Few things to consider :
        // SIMD version will use more memory footprint because it uses pre-processed vertex.
        // It might need alloc-dealloc every draw, so might be good choice to just allocating it
        // during initialize.

        // alignment and offset is already calculated for vertex.
        auto preprocess_size = sizeof(Lamp::Vec4f) * (vertex_buffer->alloc_count_ + SIMD_REGISTER_WIDTH);
        auto preprocess = Memory(preprocess_size);
        memset(preprocess.Data(), 0, preprocess_size);
        uint64_t offset = (SIMD_REGISTER_WIDTH - reinterpret_cast<uint64_t>(preprocess.Data()) % SIMD_REGISTER_WIDTH);
        auto raster_data = &preprocess.Data()[offset];

        alignas(SIMD_REGISTER_WIDTH) const float ws[8] = { 1, 1, 1, 1, 1, 1, 1, 1};

        // TODO : Maybe just do it while loading files.
        uint32_t start = 0;
        uint32_t end = 0;
        for (uint64_t i = _cmd_info.first_index_; i < index_buffer->count_; ++i) {
            start = std::min(start, *(static_cast<uint32_t*>(index_buffer->data_) + i));
            end = std::max(end, *(static_cast<uint32_t*>(index_buffer->data_) + i));
        }

        uint32_t count = index_buffer->count_ ? (end - start) + 1 : 0;
        count -= (count % SIMD_VECTOR_FETCH_PADDING);
        // Exclude last 8 elements, so we don't use padded value.

        Lamp::Mat4f merged_mat = viewport_transform * uniform->mvp;

        __m256 merged[16] = {
            _mm256_broadcast_ss(&merged_mat.c0.x),
            _mm256_broadcast_ss(&merged_mat.c1.x),
            _mm256_broadcast_ss(&merged_mat.c2.x),
            _mm256_broadcast_ss(&merged_mat.c3.x),
            _mm256_broadcast_ss(&merged_mat.c0.y),
            _mm256_broadcast_ss(&merged_mat.c1.y),
            _mm256_broadcast_ss(&merged_mat.c2.y),
            _mm256_broadcast_ss(&merged_mat.c3.y),
            _mm256_broadcast_ss(&merged_mat.c0.z),
            _mm256_broadcast_ss(&merged_mat.c1.z),
            _mm256_broadcast_ss(&merged_mat.c2.z),
            _mm256_broadcast_ss(&merged_mat.c3.z),
            _mm256_broadcast_ss(&merged_mat.c0.w),
            _mm256_broadcast_ss(&merged_mat.c1.w),
            _mm256_broadcast_ss(&merged_mat.c2.w),
            _mm256_broadcast_ss(&merged_mat.c3.w)
        };

        auto* in_x
            = reinterpret_cast<const float*>(_cmd_info.vertex_buffer_->Data());
        auto* in_y
            = reinterpret_cast<const float*>(_cmd_info.vertex_buffer_->Data())
                + 1 *_cmd_info.vertex_buffer_->alloc_count_;
        auto* in_z
            = reinterpret_cast<const float*>(_cmd_info.vertex_buffer_->Data())
                + 2 *_cmd_info.vertex_buffer_->alloc_count_;
        __m256 ww = _mm256_load_ps(ws);

        uint64_t j = 0;
        for (uint64_t i = start; i < end && j < count; i += 8, j += 8) {
            // Need more test on alignment, only tested with 2 meshes.
            // Aligned with SIMD_REGISTER_WIDTH.
            __m256 xx = _mm256_load_ps(&in_x[i]);
            __m256 yy = _mm256_load_ps(&in_y[i]);
            __m256 zz = _mm256_load_ps(&in_z[i]);

            alignas(32) __m256 out_x;
            alignas(32) __m256 out_y;
            alignas(32) __m256 out_z;

            out_x = _mm256_mul_ps(merged[0], xx);
            out_x = _mm256_fmadd_ps(merged[1], yy, out_x);
            out_x = _mm256_fmadd_ps(merged[2], zz, out_x);
            out_x = _mm256_fmadd_ps(merged[3], ww, out_x);

            out_y = _mm256_mul_ps(merged[4], xx);
            out_y = _mm256_fmadd_ps(merged[5], yy, out_y);
            out_y = _mm256_fmadd_ps(merged[6], zz, out_y);
            out_y = _mm256_fmadd_ps(merged[7], ww, out_y);

            out_z = _mm256_mul_ps(merged[8], xx);
            out_z = _mm256_fmadd_ps(merged[9], yy, out_z);
            out_z = _mm256_fmadd_ps(merged[10], zz, out_z);
            out_z = _mm256_fmadd_ps(merged[11], ww, out_z);

            __m256 clip_w;
            clip_w = _mm256_mul_ps(merged[12], xx);
            clip_w = _mm256_fmadd_ps(merged[13], yy, clip_w);
            clip_w = _mm256_fmadd_ps(merged[14], zz, clip_w);
            clip_w = _mm256_fmadd_ps(merged[15], ww, clip_w);

            out_x = _mm256_div_ps(out_x, clip_w);
            out_y = _mm256_div_ps(out_y, clip_w);
            out_z = _mm256_div_ps(out_z, clip_w);

            memcpy(&raster_data[sizeof(float) * j], &out_x, sizeof(__m256));
            memcpy(&raster_data[sizeof(float) * ((1 * vertex_buffer->alloc_count_) + j)], &out_y, sizeof(__m256));
            memcpy(&raster_data[sizeof(float) * ((2 * vertex_buffer->alloc_count_) + j)], &out_x, sizeof(__m256));
        }


        for (; j < vertex_buffer->count_; ++j) {
            alignas(16) Lamp::Vec4f v0 = Lamp::Vec4f(in_x[j], in_y[j], in_z[j], 1);
            v0 = uniform->mvp * v0;
            ClipToNdc(v0);
            NdcToWindow(viewport_transform, v0);


            memcpy(&raster_data[sizeof(float) * j], &v0.x, sizeof(float));
            memcpy(&raster_data[sizeof(float) * (1* vertex_buffer->alloc_count_ + j)], &v0.y, sizeof(float));
            memcpy(&raster_data[sizeof(float) * (2* vertex_buffer->alloc_count_ + j)], &v0.z, sizeof(float));
            memcpy(&raster_data[sizeof(float) * (3* vertex_buffer->alloc_count_ + j)], &v0.w, sizeof(float));
        }



        auto i_d = static_cast<uint8_t*>(index_buffer->data_);

        uint8_t* x = raster_data;
        uint8_t* y = &raster_data[sizeof(float) * (1 * vertex_buffer->alloc_count_)];
        uint8_t* z = &raster_data[sizeof(float) * (2 * vertex_buffer->alloc_count_)];
        uint8_t* w = &raster_data[sizeof(float) * (3 * vertex_buffer->alloc_count_)];

        for (uint64_t i = 0; i < index_buffer->count_; i += 3) {
            uint32_t i0, i1, i2;

            memcpy(&i0, &i_d[sizeof(uint32_t) * (i+0)], sizeof(uint32_t));
            memcpy(&i1, &i_d[sizeof(uint32_t) * (i+1)], sizeof(uint32_t));
            memcpy(&i2, &i_d[sizeof(uint32_t) * (i+2)], sizeof(uint32_t));

            alignas(16) Lamp::Vec4f v0, v1, v2;
            memcpy(&v0.x, &x[sizeof(float) * i0], sizeof(float));
            memcpy(&v0.y, &y[sizeof(float) * i0], sizeof(float));
            memcpy(&v0.z, &z[sizeof(float) * i0], sizeof(float));
            memcpy(&v0.w, &w[sizeof(float) * i0], sizeof(float));

            memcpy(&v1.x, &x[sizeof(float) * i1], sizeof(float));
            memcpy(&v1.y, &y[sizeof(float) * i1], sizeof(float));
            memcpy(&v1.z, &z[sizeof(float) * i1], sizeof(float));
            memcpy(&v1.w, &w[sizeof(float) * i1], sizeof(float));

            memcpy(&v2.x, &x[sizeof(float) * i2], sizeof(float));
            memcpy(&v2.y, &y[sizeof(float) * i2], sizeof(float));
            memcpy(&v2.z, &z[sizeof(float) * i2], sizeof(float));
            memcpy(&v2.w, &w[sizeof(float) * i2], sizeof(float));

            plotLine(_cmd_info, v0, v2);
            plotLine(_cmd_info, v2, v1);
            plotLine(_cmd_info, v1, v0);
        }

#else
        auto* vertices = reinterpret_cast<const Lamp::Vec3f*>(_cmd_info.vertex_buffer_->Data());
        Memory preprocess = Memory(vertex_buffer->count_ * sizeof(Lamp::Vec4f));

        uint32_t start = 0;
        uint32_t end = 0;
        for (uint64_t i = _cmd_info.first_index_; i < index_buffer->count_; ++i) {
            start = std::min(start, *(static_cast<uint32_t*>(index_buffer->data_) + i));
            end = std::max(end, *(static_cast<uint32_t*>(index_buffer->data_) + i));
        }

        for (uint64_t i = start; i <= end; ++i) {
            alignas(16) Lamp::Vec4f v0 = Lamp::Vec4f(vertices[i].x, vertices[i].y, vertices[i].z, 1.0f);
            v0 = uniform->mvp * v0;
            ClipToNdc(v0);
            NdcToWindow(viewport_transform, v0);

            memcpy(preprocess.Data() + (i * sizeof(Lamp::Vec4f)), &v0, sizeof(Lamp::Vec4f));
        }

        auto new_verticecs = reinterpret_cast<const Lamp::Vec4f*>(preprocess.Data());

        for (uint64_t i = 0; i < index_buffer->count_; i += 3) {
            auto i0 = *(static_cast<uint32_t*>(index_buffer->data_) + i);
            auto i1 = *(static_cast<uint32_t*>(index_buffer->data_) + i+1);
            auto i2 = *(static_cast<uint32_t*>(index_buffer->data_) + i+2);

            alignas(16) Lamp::Vec4f v0 = new_verticecs[i0];
            alignas(16) Lamp::Vec4f v1 = new_verticecs[i1];
            alignas(16) Lamp::Vec4f v2 = new_verticecs[i2];

            plotLine(_cmd_info, v0, v2);
            plotLine(_cmd_info, v2, v1);
            plotLine(_cmd_info, v1, v0);
        }
#endif

    }


    inline void FillInTriangle(const Lamp::Vec4f& _p,
        const Lamp::Vec4f& _v0,
        const Lamp::Vec4f& _v1,
        const Lamp::Vec4f& _v2,
        const double& _area,
        const float& _near,
        const float& _far,
        const float& _depth,
        const Image& _color_target,
        const Image& _depth_target) {
        // There are some reference about barycentric coordinate in page 479.
        // https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf
        const double w0 = EdgeFunc<double>(_v1, _v2, _p)/_area;
        const double w1 = EdgeFunc<double>(_v2, _v0, _p)/_area;
        const double w2 = EdgeFunc<double>(_v0, _v1, _p)/_area;

        // If inside triangle
        if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
            const double d0 = _v0.z;
            const double d1 = _v1.z;
            const double d2 = _v2.z;

            // Imitating vertex color attribute.
            const double red   = 1.0 * d0;
            const double green = 1.0 * d1;
            const double blue  = 1.0 * d2;

            // It should be in a form of fma, but these lines are simplified
            // Because it is blue, green and red.

            double p_interp[3] = {};
            p_interp[0] = w0 * blue;
            p_interp[1] = w1 * green;
            p_interp[2] = w2 * red;
            const double z_interp = w0 * d0 + w1 * d1 + w2 * d2;

            const uint8_t color[] = {static_cast<uint8_t>(255.0 * p_interp[0] / z_interp),
                               static_cast<uint8_t>(255.0 * p_interp[1] / z_interp),
                               static_cast<uint8_t>(255.0 * p_interp[2] / z_interp)};

            // Note that I can skip depth test here because
            // I've already did early depth.
            const double double_fp_depth = z_interp;

            // Clip near/far plane.
            if (z_interp < _near || z_interp > _far)
                return;

            if (_depth < double_fp_depth) {
                void* color_ptr = _color_target.Data()
                + (_color_target.Width()
                    * static_cast<uint32_t>(_p.y)
                    + static_cast<uint32_t>(_p.x))
                        * _color_target.Stride();

                memcpy(color_ptr, color, _color_target.Stride());
                void* depth_ptr = _depth_target.Data()
                                + (_depth_target.Width()
                                * static_cast<uint32_t>(_p.y)
                                + static_cast<uint32_t>(_p.x))
                                    * _depth_target.Stride();

                const auto f_depth = static_cast<float>(double_fp_depth);
                memcpy(depth_ptr, &f_depth, _depth_target.Stride());
            }
        }
    }
    inline void DrawRasterShader(const RenderCmdInfo& _cmd_info) {
#ifdef USE_SIMD
        auto& vertex_buffer = _cmd_info.vertex_buffer_;
        auto& index_buffer = _cmd_info.index_buffer_;
        const auto& uniform = dynamic_cast<const Render::UMvp*>(_cmd_info.uniform_);

        auto& view_port = _cmd_info.view_port_;
        Lamp::Mat4f viewport_transform = ViewportTransform(*view_port);

        const auto raster_alloc_size
            = sizeof(Lamp::Vec4f) * (vertex_buffer->alloc_count_ + SIMD_REGISTER_WIDTH);
        auto raster_alloc = Memory(raster_alloc_size);
        auto raster_buffer = raster_alloc.Data();

        const uint64_t offset = SIMD_REGISTER_WIDTH - reinterpret_cast<uint64_t>(raster_buffer) % SIMD_REGISTER_WIDTH;
        auto raster_data = &raster_buffer[offset];

        auto& color_target = _cmd_info.render_info_->_color_att->image_;
        auto& depth_target = _cmd_info.render_info_->_depth_att->image_;

        const int left = static_cast<int>(view_port->x);
        const int top  = static_cast<int>(view_port->y);
        const auto width = static_cast<uint32_t>(view_port->width);
        const auto height = static_cast<uint32_t>(view_port->height);
        alignas(SIMD_REGISTER_WIDTH) const float ws[8] = { 1, 1, 1, 1, 1, 1, 1, 1};

        LoadOp(_cmd_info.render_info_->_color_att, view_port, width, height);
        LoadOp(_cmd_info.render_info_->_depth_att, view_port, width, height);

        uint32_t start = 0;
        uint32_t end = 0;
        for (uint64_t i = _cmd_info.first_index_; i < index_buffer->count_; ++i) {
            start = std::min(start, *(static_cast<uint32_t*>(index_buffer->data_) + i));
            end = std::max(end, *(static_cast<uint32_t*>(index_buffer->data_) + i));
        }

        uint32_t count = index_buffer->count_ ? end - start : 0;
        Lamp::Mat4f merged_mat = viewport_transform * uniform->mvp;

        __m256 merged[16] = {
            _mm256_broadcast_ss(&merged_mat.c0.x),
            _mm256_broadcast_ss(&merged_mat.c1.x),
            _mm256_broadcast_ss(&merged_mat.c2.x),
            _mm256_broadcast_ss(&merged_mat.c3.x),
            _mm256_broadcast_ss(&merged_mat.c0.y),
            _mm256_broadcast_ss(&merged_mat.c1.y),
            _mm256_broadcast_ss(&merged_mat.c2.y),
            _mm256_broadcast_ss(&merged_mat.c3.y),
            _mm256_broadcast_ss(&merged_mat.c0.z),
            _mm256_broadcast_ss(&merged_mat.c1.z),
            _mm256_broadcast_ss(&merged_mat.c2.z),
            _mm256_broadcast_ss(&merged_mat.c3.z),
            _mm256_broadcast_ss(&merged_mat.c0.w),
            _mm256_broadcast_ss(&merged_mat.c1.w),
            _mm256_broadcast_ss(&merged_mat.c2.w),
            _mm256_broadcast_ss(&merged_mat.c3.w)
        };

        auto* in_x
            = reinterpret_cast<const float*>(_cmd_info.vertex_buffer_->Data());
        auto* in_y
            = reinterpret_cast<const float*>(_cmd_info.vertex_buffer_->Data())
                + 1 *_cmd_info.vertex_buffer_->alloc_count_;
        auto* in_z
            = reinterpret_cast<const float*>(_cmd_info.vertex_buffer_->Data())
                + 2 *_cmd_info.vertex_buffer_->alloc_count_;
        __m256 ww = _mm256_load_ps(ws);

        uint64_t out_itr = 0;
        for (uint64_t i = start; i <= end && out_itr <= count; i += 8, out_itr += 8) {
            __m256 xx = _mm256_load_ps(&in_x[i]);
            __m256 yy = _mm256_load_ps(&in_y[i]);
            __m256 zz = _mm256_load_ps(&in_z[i]);

            alignas(32) __m256 out_x;
            alignas(32) __m256 out_y;
            alignas(32) __m256 out_z;

            out_x = _mm256_mul_ps(merged[0], xx);
            out_x = _mm256_fmadd_ps(merged[1], yy, out_x);
            out_x = _mm256_fmadd_ps(merged[2], zz, out_x);
            out_x = _mm256_fmadd_ps(merged[3], ww, out_x);

            out_y = _mm256_mul_ps(merged[4], xx);
            out_y = _mm256_fmadd_ps(merged[5], yy, out_y);
            out_y = _mm256_fmadd_ps(merged[6], zz, out_y);
            out_y = _mm256_fmadd_ps(merged[7], ww, out_y);

            out_z = _mm256_mul_ps(merged[8], xx);
            out_z = _mm256_fmadd_ps(merged[9], yy, out_z);
            out_z = _mm256_fmadd_ps(merged[10], zz, out_z);
            out_z = _mm256_fmadd_ps(merged[11], ww, out_z);

            __m256 clip_w;
            clip_w = _mm256_mul_ps(merged[12], xx);
            clip_w = _mm256_fmadd_ps(merged[13], yy, clip_w);
            clip_w = _mm256_fmadd_ps(merged[14], zz, clip_w);
            clip_w = _mm256_fmadd_ps(merged[15], ww, clip_w);

            out_x = _mm256_div_ps(out_x, clip_w);
            out_y = _mm256_div_ps(out_y, clip_w);
            out_z = _mm256_div_ps(out_z, clip_w);

            // Reused m256 full of 1s.
            out_z = _mm256_div_ps(ww, out_z);


            memcpy(&raster_data[sizeof(float) * out_itr], &out_x, sizeof(__m256));
            memcpy(&raster_data[sizeof(float) * ((1 * vertex_buffer->alloc_count_) + out_itr)], &out_y, sizeof(__m256));
            memcpy(&raster_data[sizeof(float) * ((2 * vertex_buffer->alloc_count_) + out_itr)], &out_z, sizeof(__m256));
        }
        auto i_d = static_cast<uint8_t*>(index_buffer->data_);

        auto* backface_culling
            = _cmd_info.pipeline_->front_face_ == WindingOrder::CCW ?
                BackfaceCullCCW : BackfaceCullCW;
        uint8_t* xs = raster_data;
        uint8_t* ys = &raster_data[sizeof(float) * (1 * vertex_buffer->alloc_count_)];
        uint8_t* zs = &raster_data[sizeof(float) * (2 * vertex_buffer->alloc_count_)];

        for (uint64_t i = 0; i < index_buffer->count_; i += 3) {
            uint32_t i0, i1, i2;

            memcpy(&i0, &i_d[sizeof(uint32_t) * (i+0)], sizeof(uint32_t));
            memcpy(&i1, &i_d[sizeof(uint32_t) * (i+1)], sizeof(uint32_t));
            memcpy(&i2, &i_d[sizeof(uint32_t) * (i+2)], sizeof(uint32_t));

            alignas(16) Lamp::Vec4f v0, v1, v2;
            memcpy(&v0.x, &xs[sizeof(float) * i0], sizeof(float));
            memcpy(&v0.y, &ys[sizeof(float) * i0], sizeof(float));
            memcpy(&v0.z, &zs[sizeof(float) * i0], sizeof(float));
            memcpy(&v0.w, &ws[0], sizeof(float));

            memcpy(&v1.x, &xs[sizeof(float) * i1], sizeof(float));
            memcpy(&v1.y, &ys[sizeof(float) * i1], sizeof(float));
            memcpy(&v1.z, &zs[sizeof(float) * i1], sizeof(float));
            memcpy(&v1.w, &ws[0], sizeof(float));

            memcpy(&v2.x, &xs[sizeof(float) * i2], sizeof(float));
            memcpy(&v2.y, &ys[sizeof(float) * i2], sizeof(float));
            memcpy(&v2.z, &zs[sizeof(float) * i2], sizeof(float));
            memcpy(&v2.w, &ws[0], sizeof(float));


            AABB2i aabb;
            aabb.min = {
                static_cast<int>(std::min(std::min(v0.x, v1.x), v2.x)),
                static_cast<int>(std::min(std::min(v0.y, v1.y), v2.y))
            };
            aabb.max = {
                static_cast<int>(std::max(std::max(v0.x, v1.x), v2.x)) + 1,
                static_cast<int>(std::max(std::max(v0.y, v1.y), v2.y)) + 1
            };

            float aabb_depth = std::min(std::min(v0.z, v1.z), v2.z);
            aabb.min.x = std::max(aabb.min.x, left);
            aabb.min.y = std::max(aabb.min.y, top);
            aabb.max.x
                = std::min(aabb.max.x, static_cast<int>(left + width) -1);
            aabb.max.y
                = std::min(aabb.max.y, static_cast<int>(top + height) -1);

            LAMPASSERT(aabb.max.x < 0, "AABB Out of bound");
            LAMPASSERT(aabb.max.y < 0, "AABB Out of bound");

            // Near/far plane clipping
            if (aabb_depth < view_port->near
             || aabb_depth > view_port->far) {
                continue;
             }

            // Cross product == Area of parallelogram made with the area of triangle * 2.
            // Note that this edge function basically does pseudo-cross product.
            double area;
            area = EdgeFunc<double>(v0, v1, v2);
            // Back face culling
            if (!backface_culling(area))
                continue;

            for (int j = aabb.min.y; j < aabb.max.y; ++j) {
                for (int k = aabb.min.x; k < aabb.max.x; ++k) {
                    Lamp::Vec4f p = {static_cast<float>(k), static_cast<float>(j), 0, 0};

                    void* depth_ptr = depth_target.Data()
                        + (depth_target.Width()
                        * static_cast<uint32_t>(p.y)
                        + static_cast<uint32_t>(p.x))
                            * depth_target.Stride();
                    float depth;
                    memcpy(&depth, depth_ptr, sizeof(float));

                    // TODO : Note that operator should be interchangeable
                    // following the depth operations.
                    // C early depth test
                    if (aabb_depth < depth) {
                        continue;
                    }
                    FillInTriangle(p,v0, v1, v2, area,
                                    view_port->near, view_port->far, depth,
                                    color_target, depth_target);
                }
            }
        }
#else
        auto& vertex_buffer = _cmd_info.vertex_buffer_;
        auto& index_buffer = _cmd_info.index_buffer_;
        const auto& uniform = dynamic_cast<const Render::UMvp*>(_cmd_info.uniform_);

        auto& view_port = _cmd_info.view_port_;
        Lamp::Mat4f viewport_transform = ViewportTransform(*view_port);

        auto* vertices = reinterpret_cast<const Lamp::Vec3f*>(_cmd_info.vertex_buffer_->Data());
        auto raster_alloc = Memory(vertex_buffer->count_ * sizeof(Lamp::Vec4f));
        auto raster_data = raster_alloc.Data();

        auto& color_target = _cmd_info.render_info_->_color_att->image_;
        auto& depth_target = _cmd_info.render_info_->_depth_att->image_;

        const int x = static_cast<int>(view_port->x);
        const int y = static_cast<int>(view_port->y);
        const auto width = static_cast<uint32_t>(view_port->width);
        const auto height = static_cast<uint32_t>(view_port->height);

        LoadOp(_cmd_info.render_info_->_color_att, view_port, width, height);
        LoadOp(_cmd_info.render_info_->_depth_att, view_port, width, height);


        uint32_t start = 0;
        uint32_t end = 0;
        for (uint64_t i = _cmd_info.first_index_; i < index_buffer->count_; ++i) {
            start = std::min(start, *(static_cast<uint32_t*>(index_buffer->data_) + i));
            end = std::max(end, *(static_cast<uint32_t*>(index_buffer->data_) + i));
        }

        for (uint64_t i = start; i <= end; ++i) {
            alignas(16) auto v0 = Lamp::Vec4f(vertices[i].x, vertices[i].y, vertices[i].z, 1.0f);
            v0 = uniform->mvp * v0;

            ClipToNdc(v0);
            NdcToWindow(viewport_transform, v0);

            v0.z = 1.0f / v0.z;
            memcpy(&raster_data[i * sizeof(Lamp::Vec4f)], &v0, sizeof(Lamp::Vec4f));
        }

        auto* backface_culling
            = _cmd_info.pipeline_->front_face_ == WindingOrder::CCW ?
                BackfaceCullCCW : BackfaceCullCW;

        auto new_vertices = reinterpret_cast<const Lamp::Vec4f*>(raster_data);
        for (uint64_t i = 0; i < index_buffer->count_; i += 3) {
            auto i0 = *(static_cast<uint32_t*>(index_buffer->data_) + i);
            auto i1 = *(static_cast<uint32_t*>(index_buffer->data_) + i+1);
            auto i2 = *(static_cast<uint32_t*>(index_buffer->data_) + i+2);

            alignas(16) Lamp::Vec4f v0 = new_vertices[i0];
            alignas(16) Lamp::Vec4f v1 = new_vertices[i1];
            alignas(16) Lamp::Vec4f v2 = new_vertices[i2];

            double area;
            area = EdgeFunc<double>(v0, v1, v2);
            // Back face culling
            if (!backface_culling(area))
                continue;

            AABB2i aabb;
            aabb.min = {
                static_cast<int>(std::min(std::min(v0.x, v1.x), v2.x)),
                static_cast<int>(std::min(std::min(v0.y, v1.y), v2.y))
            };
            aabb.max = {
                static_cast<int>(std::max(std::max(v0.x, v1.x), v2.x)) + 1,
                static_cast<int>(std::max(std::max(v0.y, v1.y), v2.y)) + 1
            };

            float aabb_depth = std::min(std::min(v0.z, v1.z), v2.z);
            aabb.min.x = std::max(aabb.min.x, x);
            aabb.min.y = std::max(aabb.min.y, y);

            aabb.max.x = std::min(aabb.max.x, static_cast<int>(x + width) -1);
            aabb.max.y = std::min(aabb.max.y, static_cast<int>(y + height) -1);

            LAMPASSERT(aabb.max.x < 0, "AABB Out of bound");
            LAMPASSERT(aabb.max.y < 0, "AABB Out of bound");

            if (aabb_depth < view_port->near
             || aabb_depth > view_port->far) {
                continue;
            }


            // Early depth test
            // A pass, clip anything closer than near plane.
            // if (aabb_depth < near)
            //    clip
            //
            // B pass, clip anything further than far plane.
            // else if (aabb_depth > far)
            //    clip
            //
            // C pass, exclude any triangle that are further.
            // else if (aabb_depth > depth)
            //    clip
            // D pass, exclude any pixels that are further.
            // else if (actual depth > pixel_depth)
            //    clip

            // Cross product == Area of parallelogram made with the area of triangle * 2.
            // Note that this edge function basically does pseudo-cross product.
            for (int j = aabb.min.y; j < aabb.max.y; ++j) {
                for (int k = aabb.min.x; k < aabb.max.x; ++k) {
                    Lamp::Vec4f p{static_cast<float>(k),
                                  static_cast<float>(j),
                                  0, 0};

                    void* depth_ptr = depth_target.Data()
                                    + (depth_target.Width()
                                    * static_cast<uint32_t>(p.y)
                                    + static_cast<uint32_t>(p.x))
                                        * depth_target.Stride();
                    float depth;
                    memcpy(&depth, depth_ptr, sizeof(float));


                    // TODO : Note that operator should be interchangeable
                    // following the depth operations.
                    // C early depth test
                    if (aabb_depth < depth) {
                        continue;
                    }

                    FillInTriangle(p,v0, v1, v2, area,
                            view_port->near, view_port->far, depth,
                            color_target, depth_target);
                }
            }
        }
#endif
    }
}


inline void Render::Draw(const RenderCmdInfo& _cmd_info) {
    switch (_cmd_info.uniform_->sType) {
        case ShaderName::PointShader:
            RenderImpl::DrawPointShader(_cmd_info);
            break;
        case ShaderName::LineShader:
            RenderImpl::DrawTriangleLineShader(_cmd_info);
            break;
        case ShaderName::RasterShader:
            RenderImpl::DrawRasterShader(_cmd_info);
            break;
        case ShaderName::Count:
            // Not implemented
            break;
        default: ;
    }
}