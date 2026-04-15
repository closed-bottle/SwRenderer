#ifndef TINYTINYRENDERER_FILEREADER_H
#define TINYTINYRENDERER_FILEREADER_H
#include "FileFormats.h"
#include "special-lamp/lampString.h++"
#include "../renderUtils/Geometry.h"

class FileReader {
public:
    template<FFormat FF>
    static void LoadGeometryFile(const Lamp::String& _path, Geometry& _out);
};


#include "FileReader_impl.hpp"

#endif //TINYTINYRENDERER_FILEREADER_H