#ifndef ENGINESWAP_PosixRING
#define ENGINESWAP_PosixRING

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"
#include "env/io_posix.h"
#include "rocksdb/env.h"
#include "util/thread_local.h"
namespace ROCKSDB_NAMESPACE {

IOStatus NewRandomAccessFilePosix(const std::string& f,
                                  const FileOptions& file_opts,
                                  std::unique_ptr<FSRandomAccessFile>* r,
                                  IODebugContext* dbg);

IOStatus NewSequentialFilePosix(const std::string& f,
                                const FileOptions& file_opts,
                                std::unique_ptr<FSSequentialFile>* r,
                                IODebugContext* dbg);

IOStatus NewWritableFilePosix(const std::string& f,
                              const FileOptions& file_opts,
                              std::unique_ptr<FSWritableFile>* r,
                              IODebugContext* dbg);

class RandomAccessFilePosix : public PosixRandomAccessFile {
 public:
  RandomAccessFilePosix(const std::string& fname, int nfd,
                        size_t nlogical_block_size, const EnvOptions& noptions) :
                        PosixRandomAccessFile(fname, nfd, nlogical_block_size, noptions
#if defined(ROCKSDB_IOURING_PRESENT)
                        ,
                        nullptr
#endif
  )
  {

  }
   IOStatus Read(uint64_t offset, size_t n, const IOOptions& opts, Slice* result,
                char* scratch, IODebugContext* dbg) const override;
};

class SequentialFilePosix : public PosixSequentialFile {
 public:
  SequentialFilePosix(const std::string& fname, FILE* nfile, int nfd,
                      size_t nlogical_block_size, const EnvOptions& noptions) :
                      PosixSequentialFile(fname, nfile, nfd, nlogical_block_size, noptions)
  {

  }
  IOStatus Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
                IODebugContext* dbg) override;
};

class WritableFilePosix : public PosixWritableFile {
 public:
  WritableFilePosix(const std::string& fname, int nfd,
                    size_t nlogical_block_size, const EnvOptions& noptions) :
                    PosixWritableFile(fname, nfd, nlogical_block_size, noptions)
  {

  }
  IOStatus Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) override;
};

}
#endif
