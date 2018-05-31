#include "ion_buffer.h"

#include <string.h>

BufferData::BufferData()
    : width_(0), height_(0), vir_addr_(nullptr), mem_size_(0) {
  memset(&update_timeval_, 0, sizeof(update_timeval_));
}

IonBuffer::IonBuffer(BufferData& buf_data) : valid_data_size_(0), user_flag_(0) {
  data_ = buf_data;
}

IonBuffer::~IonBuffer() {}

