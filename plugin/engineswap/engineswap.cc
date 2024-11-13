#include "engineswap.h"

#include <rocksdb/utilities/object_registry.h>

namespace ROCKSDB_NAMESPACE {

extern "C" FactoryFunc<FileSystem> engineswap_reg;

FactoryFunc<FileSystem> engineswap_reg =
    ObjectLibrary::Default()->AddFactory<FileSystem>(
        "engineswap",
        [](const std::string& /* uri */, std::unique_ptr<FileSystem>* f,
           std::string* /* errmsg */) {
          *f = NewEngineSwapFileSystem();
          return f->get();
        });


class EngineSwapFileSystem : public FileSystemWrapper {
 public:
  EngineSwapFileSystem(std::shared_ptr<FileSystem> t) : FileSystemWrapper(t) {}

  const char* Name() const override { return "EngineFileSystem"; }
};

std::unique_ptr<FileSystem>
NewEngineSwapFileSystem() {
  return std::unique_ptr<FileSystem>(
      new EngineSwapFileSystem(FileSystem::Default()));
}

}  // namespace ROCKSDB_NAMESPACE
