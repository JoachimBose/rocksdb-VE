#ifndef ENGINESWAP_PosixRING
#define ENGINESWAP_PosixRING

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"
#include "env/io_posix.h"
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
                        PosixRandomAccessFile(fname, nfd, nlogical_block_size, noptions)
  {

  }
};

class SequentialFilePosix : public PosixSequentialFile {
 public:
  SequentialFilePosix(const std::string& fname, FILE* nfile, int nfd,
                      size_t nlogical_block_size, const EnvOptions& noptions) :
                      PosixSequentialFile(fname, nfile, nfd, nlogical_block_size, noptions)
  {

  }
};

class WritableFilePosix : public PosixWritableFile {
 public:
  WritableFilePosix(const std::string& fname, int nfd,
                    size_t nlogical_block_size, const EnvOptions& noptions) :
                    PosixWritableFile(fname, nfd, nlogical_block_size, noptions)
  {

  }
};

}
#endif
