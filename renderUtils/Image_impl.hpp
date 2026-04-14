#include "Image.h"


template<PixelFormat F>
void Image<F>::FillDiffDebug() const {
    // Fill in gradient.
    // most likely for debug purpose.
    // Note that is is actually nothing near to the gradient, but visualizing
    // direction.
    Texel<F> t = {};
    uint8_t* buff = reinterpret_cast<uint8_t*>(&t);

    Texel<F>* data = reinterpret_cast<Texel<F>*>(Data());
    for (size_t i = 0; i < n_pixels_; ++i) {
        if (i % 3 == 0) {
            buff[sizeof(t) -1] = 0;
            buff[0] = 255;
        }
        else {
            buff[i%sizeof(t)] = buff[i%sizeof(t) -1];
            buff[i%sizeof(t) -1] = 0;
        }

        data[i] = t;
    }
}

template<PixelFormat F>
void Image<F>::FillImage(Texel<F> _clear) const {
    // Fill in same color.
    // Possibly can be used to clear buffer.

    Texel<F>* data = reinterpret_cast<Texel<F>*>(Data());
    for (size_t i = 0; i < n_pixels_; ++i) {
        data[i] = _clear;
    }
}
