#pragma once
#include "Render.h"
#include "RenderInfo.h"
#include "Viewport.h"

inline void CommandBuff::Execute() {
    RenderCmdInfo cmd_info;

    Lamp::Vector<const VertexBuffer*> vertex_buffers;
    Lamp::Vector<const IndexBuffer*> index_buffers;

    for (auto& exe : execution_list_) {
        switch (exe.type_) {
            case CmdType::SetViewport:
                cmd_info.view_port_ = static_cast<const Viewport*>(exe.data_);
                break;
            case CmdType::SetRenderInfo:
                cmd_info.render_info_ = static_cast<const RenderInfo*>(exe.data_);
                break;
            case CmdType::BindPipeline:
                cmd_info.pipeline_ = static_cast<const Pipeline*>(exe.data_);
                break;
            case CmdType::BindUniform:
                cmd_info.uniform_ = static_cast<const Render::ShaderFootprint*>(exe.data_);
                break;
            case CmdType::BindVertexBuffer:
                vertex_buffers.push_back(static_cast<const VertexBuffer*>(exe.data_));
                cmd_info.vertex_buffer_ = vertex_buffers.data();
                break;
            case CmdType::BindIndexBuffer:
                index_buffers.push_back(static_cast<const IndexBuffer*>(exe.data_));
                cmd_info.index_buffer_ = index_buffers.data();
                break;
            case CmdType::BindHeap:
                cmd_info.heap_ = static_cast<const uint8_t*>(exe.data_);
            case CmdType::DrawIndexed:
                // Color
                // It should query number of attachmenets depending on the shader.
                cmd_info.first_index_ = (reinterpret_cast<uint64_t>(exe.data_));
                Render::Draw(cmd_info);
                // Depth
                break;
            default:
                // Handle error.
                break;
        }
    }
}
