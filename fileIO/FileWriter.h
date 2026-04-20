#ifndef TINYTINYRENDERER_FILEWRITER_H
#define TINYTINYRENDERER_FILEWRITER_H

#include "../renderUtils/Image.h"
#include "FileFormats.h"
#include <string>


class FileWriter {
public:
    template<FFormat FF>
    static void WriteImageToFile(std::string _path, const Image& _image);
};

#include "FileWriter_impl.hpp"

#endif //TINYTINYRENDERER_FILEWRITER_H