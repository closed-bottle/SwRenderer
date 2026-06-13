
#ifndef TINYTINYRENDERER_MEMORY_H
#define TINYTINYRENDERER_MEMORY_H

#include "special-lamp/lampVector.h++"
#include <cstdint>

class Memory {
  Lamp::Vector<uint8_t> bytes_;

public:
  Memory() = delete;

  explicit Memory(const size_t _size) { bytes_.reserve(_size); };

  [[nodiscard]] size_t Size() const { return bytes_.size(); }
  uint8_t *Data() { return bytes_.data(); }
  [[nodiscard]] const uint8_t *Data() const { return bytes_.data(); }
};

#endif // TINYTINYRENDERER_MEMORY_H