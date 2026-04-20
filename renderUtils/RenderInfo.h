#ifndef TINYTINYRENDERER_RENDERINFO_H
#define TINYTINYRENDERER_RENDERINFO_H
#include "AttachmentInfo.h"

struct RenderInfo {
    uint8_t _color_count;
    AttInfo* _color_att;
    AttInfo* _depth_att;
};

#endif //TINYTINYRENDERER_RENDERINFO_H