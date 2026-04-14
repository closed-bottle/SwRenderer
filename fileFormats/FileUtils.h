#ifndef TINYTINYRENDERER_FILEUTILS_H
#define TINYTINYRENDERER_FILEUTILS_H

#include "../renderUtils/Image.h"

class FileUtils {
public:
    template<size_t RunLength, PixelFormat PF>
    static Vector<uint8_t> RLE(const Image<PF>& _image);

    static void PushBytes(Vector<uint8_t>& _target, uint8_t _stride, const uint8_t* _data);
};

#include "FileUtils_impl.hpp"

#endif //TINYTINYRENDERER_FILEUTILS_H