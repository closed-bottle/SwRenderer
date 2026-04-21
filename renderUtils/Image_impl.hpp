#include "Image.h"


void Image::FillDiffDebug() const {
    // Fill in gradient.
    // most likely for debug purpose.
    // Note that is is actually nothing near to the gradient, but visualizing
    // direction.
    uint8_t* buff = new uint8_t[stride_];
    uint8_t* data = static_cast<uint8_t*>(Data());
    for (size_t i = 0; i < n_pixels_; ++i) {
        if (i % ElementCount(format_) == 0) {
            buff[stride_ -1] = 0;
            buff[0] = 255;
        }
        else {
            buff[i%stride_] = buff[i%stride_ -1];
            buff[i%stride_ -1] = 0;
        }

        memcpy(data, buff, stride_);
    }

    delete[] buff;
}

void Image::FillImage(const Texel& _clear) const {
    // Fill in same color.
    // Possibly can be used to clear buffer.

    uint8_t* data = static_cast<uint8_t*>(Data());
    for (size_t i = 0; i < n_pixels_; ++i) {
        memcpy(data, (void*)&_clear, stride_);
    }
}
