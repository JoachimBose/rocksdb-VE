#pragma once
#ifndef ENGINESWAP_H

#define ENGINESWAP_H

#include <string>
#include "rocksdb/file_system.h"
#include "aioengine.h"
#include "iouengine.h"
#include "posixengine.h"
#include "iounvengine.h"
#include "filedummies.h"

#include "rocksdb/file_system.h"
#include "rocksdb/utilities/object_registry.h"

namespace ROCKSDB_NAMESPACE {

    // Returns a `FileSystem` that is able to swap the storage backend from posix to libaio or io_uring
    std::unique_ptr<ROCKSDB_NAMESPACE::FileSystem>
    NewEngineSwapFileSystem(std::string engine_type);

    class EngineSwapFileSystem : public FileSystemWrapper{
    private:

        IOStatus (*NewRandomAccessFileStub)(const std::string& f, 
                                    const FileOptions& file_opts,
                                    std::unique_ptr<FSRandomAccessFile>* r,
                                    IODebugContext* dbg);

        IOStatus (*NewSequentialFileStub)(const std::string& f, 
                                    const FileOptions& file_opts,
                                    std::unique_ptr<FSSequentialFile>* r,
                                    IODebugContext* dbg);

        IOStatus (*NewWritableFileStub)(const std::string& f, 
                                    const FileOptions& file_opts,
                                    std::unique_ptr<FSWritableFile>* r,
                                    IODebugContext* dbg);
        std::string engine_type;
      
        inline static thread_local std::unique_ptr<AioRing> aioRing; //should be initialised to zeros
        inline static thread_local std::unique_ptr<IouRing> ring; //should be initialised to zeros
        
    public:
        const char* Name() const override;
        static std::unique_ptr<AioRing>* getAioRing();        
        static std::unique_ptr<IouRing>* getRing();

        EngineSwapFileSystem(std::shared_ptr<FileSystem> t, std::string nengine_type);
        IOStatus NewSequentialFile(const std::string& f, const FileOptions& file_opts,
                             std::unique_ptr<FSSequentialFile>* r,
                             IODebugContext* dbg) override;
        IOStatus NewRandomAccessFile(const std::string& f,
                               const FileOptions& file_opts,
                               std::unique_ptr<FSRandomAccessFile>* r,
                               IODebugContext* dbg) override;
        IOStatus NewWritableFile(const std::string& f,
                           const FileOptions& file_opts,
                           std::unique_ptr<FSWritableFile>* r,
                           IODebugContext* dbg) override;
    };
    

};  // namespace ROCKSDB_NAMESPACE

#endif

