#ifndef TINYTINYRENDERER_COMMANDBUFF_H
#define TINYTINYRENDERER_COMMANDBUFF_H
#include "AttachmentInfo.h"
#include "Pipeline.h"
#include "Viewport.h"
#include "RenderInfo.h"
#include "Render.h"
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

    template<PixelFormat color_format, PixelFormat depth_format>
    struct RenderCmdInfo {
        const Viewport* view_port_;
        const RenderInfo<color_format, depth_format>* render_info_;
        const Pipeline* pipeline_;
        const Render::ShaderFootprint* uniform_;
        const VertexBuffer* vertex_buffer_;
        const IndexBuffer* index_buffer_;
        const WindingOrder* front_face_;
        const ShaderName* shader_;
    };


    bool is_active_ = false;
    bool is_rendering_ = false;

    Lamp::list<CmdBlock> execution_list_;

    template<PixelFormat color_format, PixelFormat depth_format>
    void Execute_impl();
public:
    bool IsExecutable() const {return !is_active_ && !is_rendering_;}

    template<PixelFormat color_format, PixelFormat depth_format>
    void Execute() {
        Execute_impl<color_format, depth_format>();
    }
};

#include "CommandBuff_impl.hpp"

#endif //TINYTINYRENDERER_COMMANDBUFF_H