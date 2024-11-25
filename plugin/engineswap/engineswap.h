#pragma once
#ifndef ENGINESWAP_H

#define ENGINESWAP_H

#include <string>
#include "rocksdb/file_system.h"

namespace ROCKSDB_NAMESPACE {

// Returns a `FileSystem` that is able to swap the storage backend from posix to libaio or io_uring
std::unique_ptr<ROCKSDB_NAMESPACE::FileSystem>
NewEngineSwapFileSystem(std::string engine_type);
  
};  // namespace ROCKSDB_NAMESPACE

#endif