#ifndef TINYTINYRENDERER_GEOMETRY_H
#define TINYTINYRENDERER_GEOMETRY_H
#include "Memory.h"
#include "special-lamp/lampString.h++"

struct Geometry {
  Memory vertex_ = Memory(0);
  Memory vertex_normal_ = Memory(0);
  Memory uv_ = Memory(0);
  Memory index_ = Memory(0);
};

#endif // TINYTINYRENDERER_GEOMETRY_H