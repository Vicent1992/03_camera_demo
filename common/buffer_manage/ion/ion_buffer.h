#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdint.h>
#include <ion/ion.h>
#include <sys/time.h>

struct IonData {
  IonData() : client_(-1), fd_(-1), offset_(0), handle_(0) {}

  int client_;
  int fd_;
  unsigned long offset_;  // phys
  ion_user_handle_t handle_;
};

struct BufferData {
  BufferData();

  int width_;
  int height_;
  void* vir_addr_;
  size_t mem_size_;
  IonData ion_data_;
  // the time when the content of buffer update
  mutable struct timeval update_timeval_;
};

class IonBuffer {
 public:
  IonBuffer(BufferData& buf_data);
  virtual ~IonBuffer();
  inline int GetWidth() const { return data_.width_; }
  inline int GetHeight() const { return data_.height_; }
  inline size_t Size() const { return data_.mem_size_; }
  inline void* GetVirAddr() const { return data_.vir_addr_; }
  inline int GetFd() const { return data_.ion_data_.fd_; }
  inline unsigned long GetPhyOffset() const { return data_.ion_data_.offset_; }
  inline ion_user_handle_t GetHandle() const { return data_.ion_data_.handle_; }
  inline struct timeval& GetTimeval() const { return data_.update_timeval_; }
  inline void SetTimeval(struct timeval& val) { data_.update_timeval_ = val; }

  inline int GetValidDataSize() const { return valid_data_size_; }
  inline void SetValidDataSize(int size) { valid_data_size_ = size; }
  inline uint32_t GetUserFlag() const { return user_flag_; }
  inline void SetUserFlag(uint32_t flag) { user_flag_ = flag; }

  inline void Reset() {
    valid_data_size_ = 0;
    user_flag_ = 0;
  }

 protected:
  BufferData data_;

 private:
  IonBuffer(const IonBuffer&) = delete;
  IonBuffer& operator=(const IonBuffer&) = delete;

  int valid_data_size_;
  uint32_t user_flag_;
};


#endif
