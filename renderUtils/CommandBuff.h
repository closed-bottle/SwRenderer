#ifndef TINYTINYRENDERER_COMMANDBUFF_H
#define TINYTINYRENDERER_COMMANDBUFF_H

#include "special-lamp/lampList.h++"



enum class CmdType {
    Invalid,
    BeginRender,
    EndRender,
    SetViewport,
    SetRenderInfo,
    ColorAttCount,
    ColorAtt,
    DepthAtt,
    BindPipeline,
    BindUniform,
    BindVertexBuffer,
    VertexBufferBind,
    BindIndexBuffer,
    IndexBufferBind,
    BindHeap,
    BindImage,
    ImageBinding,
    DrawIndexed,
    Count
};

struct CmdBlock {
    CmdType type_ = CmdType::Invalid;
    const void* data_ = nullptr;
    const void* writable_data_ = nullptr;
};

// Ultimately, command buffers life cycle should be :
// Initial->Recording->Executable->Pending->Invalidate->Initial.
class CommandBuff {
    friend class RenderCmd;

    bool is_active_ = false;
    bool is_rendering_ = false;

    uint16_t vertex_max_binding = 0;
    uint16_t index_max_binding = 0;
    uint16_t image_max_binding = 0;
    uint16_t uniform_max_binding = 0;
    Lamp::list<CmdBlock> execution_list_;

public:
    bool IsExecutable() const {return !is_active_ && !is_rendering_;}
    void Execute();
    void Clear() {execution_list_.erase(0, execution_list_.size());}
};

#include "CommandBuff_impl.hpp"

#endif //TINYTINYRENDERER_COMMANDBUFF_H