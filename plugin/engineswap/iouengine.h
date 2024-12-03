#ifndef ENGINESWAP_IOURING
#define ENGINESWAP_IOURING

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"
#include "env/io_posix.h"
#include "rocksdb/env.h"
#include "util/thread_local.h"
#include <liburing.h>
#include <iostream>

#define QUEUE_ITEMS 4

namespace ROCKSDB_NAMESPACE {

IOStatus NewRandomAccessFileIou(const std::string& f, 
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomAccessFile>* r,
                           IODebugContext* dbg);

IOStatus NewSequentialFileIou(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSSequentialFile>* r,
                          IODebugContext* dbg);

IOStatus NewWritableFileIou(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSWritableFile>* r,
                          IODebugContext* dbg);

class IouRing{
  struct io_uring ring;
public:
  IouRing(){
    int ret = io_uring_queue_init(QUEUE_ITEMS, &ring, 0);
    if(ret != 0){
        fprintf(stderr, "que init failed %i \n errno = %i\n errno: %s\n", ret, errno, strerror(errno));
    }
    else{
      std::cout << "init success \n";
    }
  }
  ~IouRing(){
    io_uring_queue_exit(&ring);
  }
  int IouRingWrite(const Slice& data, int fd, int block_size);
  int IouRingRead(size_t n, Slice* result, char* scratch, int block_size, int fd, off_t offset);

};

class RandomAccessFileIou : public PosixRandomAccessFile {
 public:
  RandomAccessFileIou(const std::string& fname, int nfd,
                        size_t nlogical_block_size, const EnvOptions& noptions) :
                        PosixRandomAccessFile(fname, nfd, nlogical_block_size, noptions
#if defined(ROCKSDB_IOURING_PRESENT)
                        ,
                        nullptr
#endif
                        )
  {
    //constructor
  }

  IOStatus Read(uint64_t offset, size_t n, const IOOptions& opts, Slice* result,
                char* scratch, IODebugContext* dbg) const override;
};

class SequentialFileIou : public PosixSequentialFile {
 protected:
  off64_t currentByte;
 public:
  SequentialFileIou(const std::string& fname, FILE* nfile, int nfd,
                      size_t nlogical_block_size, const EnvOptions& noptions) :
                      PosixSequentialFile(fname, nfile, nfd, nlogical_block_size, noptions)
  {
    currentByte = 0;
  }
  IOStatus Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
                IODebugContext* dbg) override;
};

class WritableFileIou : public PosixWritableFile {
 
  
 public:
  WritableFileIou(const std::string& fname, int nfd,
                    size_t nlogical_block_size, const EnvOptions& noptions) :
                    PosixWritableFile(fname, nfd, nlogical_block_size, noptions)
  {
  }
  ~WritableFileIou(){
    // PosixWritableFile::~PosixWritableFile();
  }
  IOStatus Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) override;
  bool IsSyncThreadSafe() const override { return false; }

};

}
#endif
