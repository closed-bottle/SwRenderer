#ifndef TINYTINYRENDERER_PIPELINE_H
#define TINYTINYRENDERER_PIPELINE_H
#include "Image.h"
#include "Render.h"


enum class WindingOrder {
    CCW,
    CW,
    Count
};

struct Pipeline {
    WindingOrder front_face_;
    ShaderName shader_;
};




#endif //TINYTINYRENDERER_PIPELINE_H