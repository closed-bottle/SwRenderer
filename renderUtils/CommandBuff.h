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

    Lamp::list<CmdBlock> execution_list_;

public:
    bool IsExecutable() const {return !is_active_ && !is_rendering_;}
    void Execute();
    void Clear() {execution_list_.erase(0, execution_list_.size());}
};

#include "CommandBuff_impl.hpp"

#endif //TINYTINYRENDERER_COMMANDBUFF_H