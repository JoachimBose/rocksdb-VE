#pragma once

#include <rocksdb/file_system.h>

namespace ROCKSDB_NAMESPACE {

// Returns a `FileSystem` that is able to swap the storage backend from posix to libaio or io_uring
std::unique_ptr<ROCKSDB_NAMESPACE::FileSystem>
NewEngineSwapFileSystem();

}  // namespace ROCKSDB_NAMESPACE
