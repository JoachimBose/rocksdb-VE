#ifndef ENGINESWAP_IOURINGNVSQP
#define ENGINESWAP_IOURINGNVSQP

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"
#include "env/io_posix.h"
#include "rocksdb/env.h"
#include "util/thread_local.h"
#include <liburing.h>
#include <iostream>

#define QUEUE_ITEMS 4

namespace ROCKSDB_NAMESPACE {

IOStatus NewRandomAccessFileIouNvSqp(const std::string& f, 
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomAccessFile>* r,
                           IODebugContext* dbg);

IOStatus NewSequentialFileIouNvSqp(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSSequentialFile>* r,
                          IODebugContext* dbg);

IOStatus NewWritableFileIouNvSqp(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSWritableFile>* r,
                          IODebugContext* dbg);

class RandomAccessFileIouNvSqp : public PosixRandomAccessFile {
 public:
  RandomAccessFileIouNvSqp(const std::string& fname, int nfd,
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

class SequentialFileIouNvSqp : public PosixSequentialFile {
 protected:
  off64_t currentByte;
 public:
  SequentialFileIouNvSqp(const std::string& fname, FILE* nfile, int nfd,
                      size_t nlogical_block_size, const EnvOptions& noptions) :
                      PosixSequentialFile(fname, nfile, nfd, nlogical_block_size, noptions)
  {
    currentByte = 0;
  }
  IOStatus Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
                IODebugContext* dbg) override;
};

class WritableFileIouNvSqp : public PosixWritableFile {
 
  
 public:
  WritableFileIouNvSqp(const std::string& fname, int nfd,
                    size_t nlogical_block_size, const EnvOptions& noptions) :
                    PosixWritableFile(fname, nfd, nlogical_block_size, noptions)
  {
  }
  ~WritableFileIouNvSqp(){
    // PosixWritableFile::~PosixWritableFile();
  }
  using PosixWritableFile::Append;
  IOStatus Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) override;
  bool IsSyncThreadSafe() const override { return false; }

};

}
#endif
