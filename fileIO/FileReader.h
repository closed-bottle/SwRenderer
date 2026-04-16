#ifndef TINYTINYRENDERER_FILEREADER_H
#define TINYTINYRENDERER_FILEREADER_H
#include "FileFormats.h"
#include "special-lamp/lampString.h++"
#include "../renderUtils/Geometry.h"
#include "../renderUtils/Mesh.h"
class FileReader {
public:
    template<FFormat FF>
    static bool LoadGeometryFile(const Lamp::String& _path, Geometry& _out_geom, Mesh& _out_mesh);
};


#include "FileReader_impl.hpp"

#endif //TINYTINYRENDERER_FILEREADER_H