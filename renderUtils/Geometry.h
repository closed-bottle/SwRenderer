#ifndef TINYTINYRENDERER_GEOMETRY_H
#define TINYTINYRENDERER_GEOMETRY_H

#include "Memory.h"

struct Geometry {
    Lamp::Memory vertex_;
    Lamp::Memory index_;
};

#endif //TINYTINYRENDERER_GEOMETRY_H