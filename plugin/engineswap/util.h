#ifndef UTIL_ENGINESWAP

#include "rocksdb/io_status.h"
#include "rocksdb/file_system.h"
#include "rocksdb/env.h"

namespace rocksdb{
    size_t GetLogicalBlockSizeOfFd(int fd);
    size_t GetLogicalBlockSize(const std::string& fname, int fd) ;
    size_t GetLogicalBlockSizeForReadIfNeeded(
            const EnvOptions& options, const std::string& fname, int fd);
    size_t GetLogicalBlockSizeForWriteIfNeeded(
            const EnvOptions& options, const std::string& fname, int fd);
    int cloexec_flags(int flags, const EnvOptions* options);
    bool SupportsFastAllocate(int fd);
    void MaybeForceDisableMmap(int fd);
    void SetFD_CLOEXEC(int fd, const EnvOptions* options);
    IOStatus GetFileSize(const std::string& fname, const IOOptions& /*opts*/,
                        uint64_t* size, IODebugContext* /*dbg*/);   
    IOStatus OpenWritableFile(const std::string& fname,
                                    const FileOptions& options, bool reopen,
                                    std::unique_ptr<FSWritableFile>* result,
                                    IODebugContext* /*dbg*/);
}

#define UTIL_ENGINESWAP
#endif