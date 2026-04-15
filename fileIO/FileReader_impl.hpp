#include "FileReader.h"

namespace {
    void LoadOBJ(const Lamp::String& _path, Geometry &_out) {

    }
}



template<FFormat FF>
void FileReader::LoadGeometryFile(const Lamp::String& _path, Geometry &_out) {
    if (FF == FFormat::OBJ)
        LoadOBJ(_path, _out);
}
