#ifndef TINYTINYRENDERER_IMAGE_H
#define TINYTINYRENDERER_IMAGE_H

#include <vector>
#include "Memory.h"
#include "ImageFormat.h"

using namespace std;

// Developer take responsiblilty for how to handle image after delete.
// It can be reused of destroyed, but using it after freeing memory
// results in UB.
template<PixelFormat F>
class Image {
    Memory& mem_;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
    Texel<F> format_;
    size_t stride_ = 0;
    uint32_t n_pixels_ = 0;
    size_t size_in_byte_ = 0;
public:
    Image() = delete;
    explicit Image(Memory& _mem, uint32_t _width, uint32_t _height)
        : mem_(_mem), width_(_width), height_(_height), stride_(sizeof(format_)) {
        n_pixels_ = width_ * height_;
        size_in_byte_ = n_pixels_ * stride_;
    }

    size_t Stride() const { return stride_;}
    uint32_t Width() const { return width_;}
    uint32_t Height() const { return height_;}
    uint32_t NPixels() const { return n_pixels_;}
    uint8_t* ByteData() const { return mem_.Data(); }
    Texel<F>* Data() const { return reinterpret_cast<Texel<F>*>(mem_.Data()); }

    size_t SizeInByte() const { return size_in_byte_; }

    void FillDiffDebug() const;
    void FillImage(Texel<F> _clear) const;
};

#include "Image_impl.hpp"
#endif //TINYTINYRENDERER_IMAGE_H
