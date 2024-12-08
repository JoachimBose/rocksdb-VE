#ifndef ENGINESWAP_IOURINGNV
#define ENGINESWAP_IOURINGNV

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"
#include "env/io_posix.h"
#include "rocksdb/env.h"
#include "util/thread_local.h"
#include <liburing.h>
#include <iostream>

#define QUEUE_ITEMS 4

namespace ROCKSDB_NAMESPACE {

IOStatus NewRandomAccessFileIouNv(const std::string& f, 
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomAccessFile>* r,
                           IODebugContext* dbg);

IOStatus NewSequentialFileIouNv(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSSequentialFile>* r,
                          IODebugContext* dbg);

IOStatus NewWritableFileIouNv(const std::string& f, 
                          const FileOptions& file_opts,
                          std::unique_ptr<FSWritableFile>* r,
                          IODebugContext* dbg);

class RandomAccessFileIouNv : public PosixRandomAccessFile {
 public:
  RandomAccessFileIouNv(const std::string& fname, int nfd,
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

class SequentialFileIouNv : public PosixSequentialFile {
 protected:
  off64_t currentByte;
 public:
  SequentialFileIouNv(const std::string& fname, FILE* nfile, int nfd,
                      size_t nlogical_block_size, const EnvOptions& noptions) :
                      PosixSequentialFile(fname, nfile, nfd, nlogical_block_size, noptions)
  {
    currentByte = 0;
  }
  IOStatus Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
                IODebugContext* dbg) override;
};

class WritableFileIouNv : public PosixWritableFile {
 
  
 public:
  WritableFileIouNv(const std::string& fname, int nfd,
                    size_t nlogical_block_size, const EnvOptions& noptions) :
                    PosixWritableFile(fname, nfd, nlogical_block_size, noptions)
  {
  }
  ~WritableFileIouNv(){
    // PosixWritableFile::~PosixWritableFile();
  }
  using PosixWritableFile::Append;
  IOStatus Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) override;
  bool IsSyncThreadSafe() const override { return false; }

};

}
#endif
