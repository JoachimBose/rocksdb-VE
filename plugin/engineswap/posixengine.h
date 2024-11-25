#ifndef ENGINESWAP_PosixRING
#define ENGINESWAP_PosixRING

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"
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

};

#endif
