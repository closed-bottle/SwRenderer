#ifndef TINYTINYRENDERER_IMAGE_H
#define TINYTINYRENDERER_IMAGE_H

#include <vector>
#include "Memory.h"
#include "ImageFormat.h"



// Developer takes full responsibility for how to handle image after delete.
// It can be reused of destroyed, but using it after freeing memory
// results in UB.

class Image {
public:
    // Currently has no effect. In the future, it might help optimizing
    // or add features like access pattern.
    enum class Layout {
        IMAGE_LAYOUT_UNDEFINED,
        Count
    };
private:
    Memory& mem_;
    size_t offset_;
    PixelFormat format_ = PixelFormat::Invalid;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    size_t stride_ = 0;
    uint32_t n_pixels_ = 0;
    size_t size_in_byte_ = 0;
    Layout layout_;

public:
    Image() = delete;
    explicit Image(Memory& _mem, PixelFormat _format, size_t _offset, uint32_t _width, uint32_t _height)
        : mem_(_mem), offset_(_offset), format_(_format), width_(_width), height_(_height) {
        stride_ = FormatStride(_format);
        n_pixels_ = width_ * height_;
        size_in_byte_ = n_pixels_ * stride_;
    }

    size_t Stride() const { return stride_;}
    uint32_t Width() const { return width_;}
    uint32_t Height() const { return height_;}
    uint32_t NPixels() const { return n_pixels_;}
    uint8_t* ByteData() const { return mem_.Data(); }
    void* Data() const { return (mem_.Data() + offset_); }

    size_t SizeInByte() const { return size_in_byte_; }

    PixelFormat Format() const { return format_; }

    void FillDiffDebug() const;
    void FillImage(const Texel& _clear) const;
};

#include "Image_impl.hpp"
#endif //TINYTINYRENDERER_IMAGE_H
