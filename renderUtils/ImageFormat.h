#ifndef TINYTINYRENDERER_IMAGEFORMAT_H
#define TINYTINYRENDERER_IMAGEFORMAT_H
#include <cstdint>
#pragma pack(push, 1)

enum class PixelFormat : size_t {
  Invalid = 0,
  R8,
  R8G8B8,
  B8G8R8,
  D16,
  D32,
  Count
};

// I don't think it is good idea to use nameless namespace in header.
namespace ReservedImageFormatParams {
constexpr size_t elements_count[] = {
    0, // Invalid
    1, // R8
    3, // R8G8B8
    3, // B8G8R8
    1, // D16
    1, // D32
    0  // Count
};

constexpr size_t stride[] = {
    0,                                                            // Invalid
    1 * elements_count[static_cast<size_t>(PixelFormat::R8G8B8)], // R8
    1 * elements_count[static_cast<size_t>(PixelFormat::R8G8B8)], // R8G8B8
    1 * elements_count[static_cast<size_t>(PixelFormat::B8G8R8)], // B8G8R8
    2 * elements_count[static_cast<size_t>(PixelFormat::D16)],    // D16
    4 * elements_count[static_cast<size_t>(PixelFormat::D32)],    // D32
    0                                                             // Count
};
} // namespace ReservedImageFormatParams

constexpr size_t ElementCount(PixelFormat _f) {
  return ReservedImageFormatParams::elements_count[static_cast<size_t>(_f)];
}

constexpr size_t FormatStride(PixelFormat _f) {
  return ReservedImageFormatParams::stride[static_cast<size_t>(_f)];
}

struct R8G8B8 {
  uint8_t R;
  uint8_t G;
  uint8_t B;

  R8G8B8 operator+(const R8G8B8 &_r) const {
    R8G8B8 result{};
    result.R += _r.R;
    result.G += _r.G;
    result.B += _r.B;
    return result;
  }

  R8G8B8 &operator+=(const R8G8B8 &_r) {
    R += _r.R;
    G += _r.G;
    B += _r.B;
    return *this;
  }

  bool operator==(const R8G8B8 &_r) const {
    return R == _r.R && G == _r.G && B == _r.B;
  }

  bool operator!=(const R8G8B8 &_r) const {
    return R != _r.R || G != _r.G || B != _r.B;
  }
};

struct R8 {
  uint8_t R;

  R8 operator+(const R8 &_r) const {
    R8 result{};
    result.R += _r.R;

    return result;
  }

  R8 &operator+=(const R8 &_r) {
    R += _r.R;

    return *this;
  }

  bool operator==(const R8 &_r) const { return R == _r.R; }

  bool operator!=(const R8 &_r) const { return R != _r.R; }
};

struct B8G8R8 {
  uint8_t B;
  uint8_t G;
  uint8_t R;

  B8G8R8(const uint8_t _blue, const uint8_t _green, const uint8_t _red)
      : B(_blue), G(_green), R(_red) {}

  B8G8R8 operator+(const B8G8R8 &_r) const {
    B8G8R8 result{0, 0, 0};
    result.R += _r.R;
    result.G += _r.G;
    result.B += _r.B;
    return result;
  }

  B8G8R8 &operator+=(const B8G8R8 &_r) {
    R += _r.R;
    G += _r.G;
    B += _r.B;
    return *this;
  }

  bool operator==(const B8G8R8 &_r) const {
    return R == _r.R && G == _r.G && B == _r.B;
  }

  bool operator!=(const B8G8R8 &_r) const {
    return R != _r.R || G != _r.G || B != _r.B;
  }
};

struct D16 {
  uint16_t D;

  explicit D16(const uint16_t _d) : D(_d) {}

  D16 operator+(const D16 &_r) const {
    D16 result{0};
    result.D += _r.D;
    return result;
  }

  D16 &operator+=(const D16 &_r) {
    D += _r.D;
    return *this;
  }

  bool operator==(const D16 &_r) const { return D == _r.D; }

  bool operator!=(const D16 &_r) const { return D != _r.D; }
};

struct D32 {
  uint32_t D;

  explicit D32(const uint32_t _d) : D(_d) {}

  D32 operator+(const D32 &_r) const {
    D32 result{0};
    result.D += _r.D;
    return result;
  }

  D32 &operator+=(const D32 &_r) {
    D += _r.D;
    return *this;
  }

  bool operator==(const D32 &_r) const { return D == _r.D; }

  bool operator!=(const D32 &_r) const { return D != _r.D; }
};
#pragma pack(pop)
#endif // TINYTINYRENDERER_IMAGEFORMAT_H
