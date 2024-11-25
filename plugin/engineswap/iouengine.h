#ifndef ENGINESWAP_IOURING
#define ENGINESWAP_IOURING

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"
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


};
#endif
