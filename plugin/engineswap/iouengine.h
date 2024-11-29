#ifndef ENGINESWAP_IOURING
#define ENGINESWAP_IOURING

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"
#include "env/io_posix.h"
#include "rocksdb/env.h"
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

class RandomAccessFileIou : public PosixRandomAccessFile {
 public:
  RandomAccessFileIou(const std::string& fname, int nfd,
                        size_t nlogical_block_size, const EnvOptions& noptions) :
                        PosixRandomAccessFile(fname, nfd, nlogical_block_size, noptions)
  {

  }
};

class SequentialFileIou : public PosixSequentialFile {
 public:
  SequentialFileIou(const std::string& fname, FILE* nfile, int nfd,
                      size_t nlogical_block_size, const EnvOptions& noptions) :
                      PosixSequentialFile(fname, nfile, nfd, nlogical_block_size, noptions)
  {

  }
};

class WritableFileIou : public PosixWritableFile {
 public:
  WritableFileIou(const std::string& fname, int nfd,
                    size_t nlogical_block_size, const EnvOptions& noptions) :
                    PosixWritableFile(fname, nfd, nlogical_block_size, noptions)
  {

  }
  IOStatus Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) override;
};

}
#endif

