#ifndef TINYTINYRENDERER_ATTACHMENTINFO_H
#define TINYTINYRENDERER_ATTACHMENTINFO_H
#include "Image.h"

enum class LoadOp {
  LOAD_OP_CLEAR // default
};

enum class StoreOp {
  STORE_OP_STORE, // default
  STORE_OP_DONT_CARE
};

struct AttInfo {
  AttInfo(const Image &image, const LoadOp load_op, const StoreOp store_op,
          uint8_t *clear_val)
      : image_(image), load_op_(load_op), store_op_(store_op),
        clear_val_(clear_val) {}
  Image image_;
  LoadOp load_op_;
  StoreOp store_op_;
  uint8_t *clear_val_{};
};

#endif // TINYTINYRENDERER_ATTACHMENTINFO_H