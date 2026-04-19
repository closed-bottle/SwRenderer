#ifndef TINYTINYRENDERER_IMAGEFORMAT_H
#define TINYTINYRENDERER_IMAGEFORMAT_H
#include <cstdint>

#pragma pack(push, 1)

enum class PixelFormat : size_t {
    Invalid = 0,
    R8G8B8,
    B8G8R8,
    D16,
    Count
};

// I don't think it is good idea to use nameless namespace in header.
namespace {
    constexpr size_t elements_count[] = {
        0, //Invalid
        3, //R8G8B8
        3, //B8G8R8
        1, //D16
        0 //Count
    };

    constexpr size_t stride[] = {
        0, //Invalid
        8 * elements_count[static_cast<size_t>(PixelFormat::R8G8B8)], //R8G8B8
        8 * elements_count[static_cast<size_t>(PixelFormat::B8G8R8)], //B8G8R8
        16 * elements_count[static_cast<size_t>(PixelFormat::D16)], //D16
        0 //Count
    };
}

constexpr size_t ElementCount(PixelFormat _f) {
    return elements_count[static_cast<size_t>(_f)];
}

constexpr size_t FormatStride(PixelFormat _f) {
    return stride[static_cast<size_t>(_f)];
}

struct Texel {
    virtual constexpr PixelFormat Format() = 0;
};

struct R8G8B8 : Texel {
    uint8_t R;
    uint8_t G;
    uint8_t B;

    constexpr PixelFormat Format() override {
        return PixelFormat::R8G8B8;
    }

    R8G8B8 operator+(const R8G8B8& _r) const {
        R8G8B8 result;
        result.R += _r.R;
        result.G += _r.G;
        result.B += _r.B;
        return result;
    }

    R8G8B8& operator+=(const R8G8B8& _r) {
        R += _r.R;
        G += _r.G;
        B += _r.B;
        return *this;
    }

    bool operator==(const R8G8B8& _r) const {
        return R == _r.R && G == _r.G && B == _r.B;
    }

    bool operator!=(const R8G8B8& _r) const {
        return R != _r.R || G != _r.G || B != _r.B;
    }
};

struct B8G8R8 : Texel{
    uint8_t B;
    uint8_t G;
    uint8_t R;

    constexpr PixelFormat Format() override {
        return PixelFormat::B8G8R8;
    }

    B8G8R8 operator+(const B8G8R8& _r) const {
        B8G8R8 result;
        result.R += _r.R;
        result.G += _r.G;
        result.B += _r.B;
        return result;
    }

    B8G8R8& operator+=(const B8G8R8& _r) {
        R += _r.R;
        G += _r.G;
        B += _r.B;
        return *this;
    }

    bool operator==(const B8G8R8& _r) const {
        return R == _r.R && G == _r.G && B == _r.B;
    }

    bool operator!=(const B8G8R8& _r) const {
        return R != _r.R || G != _r.G || B != _r.B;
    }
};

struct D16 : Texel {
    uint16_t D;

    constexpr PixelFormat Format() override {
        return PixelFormat::D16;
    }

    D16 operator+(const D16& _r) const {
        D16 result;
        result.D += _r.D;
        return result;
    }

    D16& operator+=(const D16& _r) {
        D += _r.D;
        return *this;
    }

    bool operator==(const D16& _r) const {
        return D == _r.D;
    }

    bool operator!=(const D16& _r) const {
        return D != _r.D;
    }
};
#pragma pack(pop)
#endif //TINYTINYRENDERER_IMAGEFORMAT_H
