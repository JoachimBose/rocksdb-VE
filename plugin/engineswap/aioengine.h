#ifndef ENGINESWAP_LIBAIO
#define ENGINESWAP_LIBAIO

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"
#include "env/io_posix.h"
#include "rocksdb/env.h"
#include "util/thread_local.h"
#include <libaio.h>
#include <iostream>

#define QUEUE_ITEMS 4

namespace ROCKSDB_NAMESPACE {

IOStatus NewRandomAccessFileAio(const std::string& f, 
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomAccessFile>* r,
                           IODebugContext* dbg);

IOStatus NewSequentialFileAio(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSSequentialFile>* r,
                          IODebugContext* dbg);

IOStatus NewWritableFileAio(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSWritableFile>* r,
                          IODebugContext* dbg);

class AioRing{

  struct iocb *ioq[QUEUE_ITEMS];
  struct timespec timeout;
  struct io_event events[QUEUE_ITEMS];
  io_context_t ctx;

public:
  AioRing(){
    int ret;
    memset(&ctx, 0, sizeof(ctx));
    if((ret = io_setup(QUEUE_ITEMS, &ctx)) < 0){
      std::cout << "Aio queue init failed with code: " << -ret << "fuckt his shit!\n";
    }else{
      timeout.tv_nsec = 0;
      timeout.tv_sec = 3;
    }
  }

  ~AioRing(){
    io_destroy(ctx);
  }

  int AioRingWrite(const Slice& data, int fd, int block_size);
  int AioRingRead(size_t n, Slice* result, char* scratch, int block_size, int fd, off_t offset);

};

class RandomAccessFileAio : public PosixRandomAccessFile {
 public:
  RandomAccessFileAio(const std::string& fname, int nfd,
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

class SequentialFileAio : public PosixSequentialFile {
 protected:
  off64_t currentByte;
 public:
  SequentialFileAio(const std::string& fname, FILE* nfile, int nfd,
                      size_t nlogical_block_size, const EnvOptions& noptions) :
                      PosixSequentialFile(fname, nfile, nfd, nlogical_block_size, noptions)
  {
    currentByte = 0;
  }
  IOStatus Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
                IODebugContext* dbg) override;
};

class WritableFileAio : public PosixWritableFile {
 
  
 public:
  WritableFileAio(const std::string& fname, int nfd,
                    size_t nlogical_block_size, const EnvOptions& noptions) :
                    PosixWritableFile(fname, nfd, nlogical_block_size, noptions)
  {
  }
  ~WritableFileAio(){
    // PosixWritableFile::~PosixWritableFile();
  }
  using PosixWritableFile::Append;
  IOStatus Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) override;
  bool IsSyncThreadSafe() const override { return false; }

};

}
#endif