#pragma once
#include "Render.h"
#include "RenderInfo.h"
#include "Viewport.h"

inline void CommandBuff::Execute() {
  RenderCmdInfo cmd_info;

  Lamp::Vector<const VertexBuffer *> vertex_buffers;
  Lamp::Vector<uint16_t> vertex_bindings;
  Lamp::Vector<const IndexBuffer *> index_buffers;
  Lamp::Vector<uint16_t> index_bindings;
  Lamp::Vector<const Image *> images;
  Lamp::Vector<uint16_t> image_bindings;

  vertex_buffers.reserve(vertex_max_binding);
  index_buffers.reserve(index_max_binding);
  images.reserve(image_max_binding);
  vertex_bindings.reserve(vertex_max_binding);
  index_bindings.reserve(index_max_binding);
  image_bindings.reserve(image_max_binding);

  static uint16_t vertex_binding = 0;
  static uint16_t index_binding = 0;
  static uint16_t image_binding = 0;

  for (auto &exe : execution_list_) {
    switch (exe.type_) {
    case CmdType::SetViewport:
      cmd_info.view_port_ = static_cast<const Viewport *>(exe.data_);
      break;
    case CmdType::SetRenderInfo:
      cmd_info.render_info_ = static_cast<const RenderInfo *>(exe.data_);
      break;
    case CmdType::BindPipeline:
      cmd_info.pipeline_ = static_cast<const Pipeline *>(exe.data_);
      break;
    case CmdType::BindUniform:
      cmd_info.uniform_ =
          static_cast<const Render::ShaderFootprint *>(exe.data_);
      break;
    case CmdType::BindVertexBuffer:
      vertex_buffers[vertex_binding] =
          static_cast<const VertexBuffer *>(exe.data_);
      break;
    case CmdType::VertexBufferBind:
      vertex_binding = reinterpret_cast<uint64_t>(exe.data_);
      break;
    case CmdType::BindIndexBuffer:
      index_buffers[index_binding] =
          static_cast<const IndexBuffer *>(exe.data_);
      break;
    case CmdType::IndexBufferBind:
      index_binding = reinterpret_cast<uint64_t>(exe.data_);
      break;
    case CmdType::BindHeap:
      cmd_info.heap_ = static_cast<const uint8_t *>(exe.data_);
      break;
    case CmdType::BindImage:
      images[image_binding] = static_cast<const Image *>(exe.data_);
    case CmdType::ImageBinding:
      image_binding = reinterpret_cast<uint64_t>(exe.data_);
      break;
    case CmdType::DrawIndexed:
      // Color
      // It should query number of attachmenets depending on the shader.
      cmd_info.first_index_ = reinterpret_cast<uint64_t>(exe.data_);
      cmd_info.vertex_buffer_ = vertex_buffers.data();
      cmd_info.index_buffer_ = index_buffers.data();
      cmd_info.image_ = images.data();

      Render::Draw(cmd_info);
      // Depth
      break;
    default:
      // Handle error.
      break;
    }
  }
}
