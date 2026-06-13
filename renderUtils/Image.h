#ifndef TINYTINYRENDERER_IMAGE_H
#define TINYTINYRENDERER_IMAGE_H

#include "ImageFormat.h"
#include "Memory.h"

// Developer takes full responsibility for how to handle image after delete.
// It can be reused of destroyed, but using it after freeing memory
// results in UB.

class Image {
public:
  // Currently has no effect. In the future, it might help optimizing
  // or add features like access pattern.
  enum class Layout { IMAGE_LAYOUT_UNDEFINED, Count };

private:
  Memory &mem_;
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
  explicit Image(Memory &_mem, const PixelFormat _format, const size_t _offset,
                 const uint32_t _width, const uint32_t _height)
      : mem_(_mem), offset_(_offset), format_(_format), width_(_width),
        height_(_height), layout_() {
    stride_ = FormatStride(_format);
    n_pixels_ = width_ * height_;
    size_in_byte_ = n_pixels_ * stride_;
  }

  [[nodiscard]] size_t Stride() const { return stride_; }
  [[nodiscard]] uint32_t Width() const { return width_; }
  [[nodiscard]] uint32_t Height() const { return height_; }
  [[nodiscard]] uint32_t NPixels() const { return n_pixels_; }
  [[nodiscard]] uint8_t *ByteData() const { return mem_.Data(); }
  [[nodiscard]] uint8_t *Data() const { return (mem_.Data() + offset_); }

  [[nodiscard]] size_t SizeInByte() const { return size_in_byte_; }

  [[nodiscard]] PixelFormat Format() const { return format_; }
};

// NOLINTNEXTLINE
#include "Image_impl.hpp"
#endif // TINYTINYRENDERER_IMAGE_H
