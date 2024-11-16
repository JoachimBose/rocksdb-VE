#include "engineswap.h"

#include <rocksdb/utilities/object_registry.h>
#include <iostream>

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
  
  IOStatus NewSequentialFile(const std::string& f, const FileOptions& file_opts,
                             std::unique_ptr<FSSequentialFile>* r,
                             IODebugContext* dbg) override {
    std::cout << " NewSequentialFile\n";
    return FileSystemWrapper::NewSequentialFile(f, file_opts, r, dbg);
  }
  IOStatus NewRandomAccessFile(const std::string& f,
                               const FileOptions& file_opts,
                               std::unique_ptr<FSRandomAccessFile>* r,
                               IODebugContext* dbg) override {
    std::cout << " NewRandomAccessFile\n";
    return FileSystemWrapper::NewRandomAccessFile(f, file_opts, r, dbg);
  }
  IOStatus NewWritableFile(const std::string& f, const FileOptions& file_opts,
                           std::unique_ptr<FSWritableFile>* r,
                           IODebugContext* dbg) override {
    std::cout << " NewWritableFile\n";
    return FileSystemWrapper::NewWritableFile(f, file_opts, r, dbg);
  }
  IOStatus ReopenWritableFile(const std::string& fname,
                              const FileOptions& file_opts,
                              std::unique_ptr<FSWritableFile>* result,
                              IODebugContext* dbg) override {
    std::cout << " ReopenWritableFile\n";
    return FileSystemWrapper::ReopenWritableFile(fname, file_opts, result, dbg);
  }
  IOStatus ReuseWritableFile(const std::string& fname,
                             const std::string& old_fname,
                             const FileOptions& file_opts,
                             std::unique_ptr<FSWritableFile>* r,
                             IODebugContext* dbg) override {
    std::cout << " ReuseWritableFile\n";
    return FileSystemWrapper::ReuseWritableFile(fname, old_fname, file_opts, r, dbg);
  }
  IOStatus NewRandomRWFile(const std::string& fname,
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomRWFile>* result,
                           IODebugContext* dbg) override {
    std::cout << " NewRandomRWFile\n";
    return FileSystemWrapper::NewRandomRWFile(fname, file_opts, result, dbg);
  }
  IOStatus NewMemoryMappedFileBuffer(
      const std::string& fname,
      std::unique_ptr<MemoryMappedFileBuffer>* result) override {
    std::cout << " NewMemoryMappedFileBuffer\n";
    return FileSystemWrapper::NewMemoryMappedFileBuffer(fname, result);
  }
  // IOStatus NewDirectory(const std::string& name, const IOOptions& io_opts,
  //                       std::unique_ptr<FSDirectory>* result,
  //                       IODebugContext* dbg) override {
  //   return FileSystemWrapper::NewDirectory(name, io_opts, result, dbg);
  // }
  // IOStatus FileExists(const std::string& f, const IOOptions& io_opts,
  //                     IODebugContext* dbg) override {
  //   return FileSystemWrapper::FileExists(f, io_opts, dbg);
  // }
  // IOStatus GetChildren(const std::string& dir, const IOOptions& io_opts,
  //                      std::vector<std::string>* r,
  //                      IODebugContext* dbg) override {
  //   return FileSystemWrapper::GetChildren(dir, io_opts, r, dbg);
  // }
  // IOStatus GetChildrenFileAttributes(const std::string& dir,
  //                                    const IOOptions& options,
  //                                    std::vector<FileAttributes>* result,
  //                                    IODebugContext* dbg) override {
  //   return FileSystemWrapper::GetChildrenFileAttributes(dir, options, result, dbg);
  // }
  // IOStatus DeleteFile(const std::string& f, const IOOptions& options,
  //                     IODebugContext* dbg) override {
  //   return FileSystemWrapper::DeleteFile(f, options, dbg);
  // }
  // IOStatus Truncate(const std::string& fname, size_t size,
  //                   const IOOptions& options, IODebugContext* dbg) override {
  //   return FileSystemWrapper::Truncate(fname, size, options, dbg);
  // }
  // IOStatus CreateDir(const std::string& d, const IOOptions& options,
  //                    IODebugContext* dbg) override {
  //   return FileSystemWrapper::CreateDir(d, options, dbg);
  // }
  // IOStatus CreateDirIfMissing(const std::string& d, const IOOptions& options,
  //                             IODebugContext* dbg) override {
  //   return FileSystemWrapper::CreateDirIfMissing(d, options, dbg);
  // }
  // IOStatus DeleteDir(const std::string& d, const IOOptions& options,
  //                    IODebugContext* dbg) override {
  //   return FileSystemWrapper::DeleteDir(d, options, dbg);
  // }
  // IOStatus GetFileSize(const std::string& f, const IOOptions& options,
  //                      uint64_t* s, IODebugContext* dbg) override {
  //   return FileSystemWrapper::GetFileSize(f, options, s, dbg);
  // }

  // IOStatus GetFileModificationTime(const std::string& fname,
  //                                  const IOOptions& options,
  //                                  uint64_t* file_mtime,
  //                                  IODebugContext* dbg) override {
  //   return FileSystemWrapper::GetFileModificationTime(fname, options, file_mtime, dbg);
  // }

  // IOStatus GetAbsolutePath(const std::string& db_path, const IOOptions& options,
  //                          std::string* output_path,
  //                          IODebugContext* dbg) override {
  //   return FileSystemWrapper::GetAbsolutePath(db_path, options, output_path, dbg);
  // }

  // IOStatus RenameFile(const std::string& s, const std::string& t,
  //                     const IOOptions& options, IODebugContext* dbg) override {
  //   return FileSystemWrapper::RenameFile(s, t, options, dbg);
  // }

  // IOStatus LinkFile(const std::string& s, const std::string& t,
  //                   const IOOptions& options, IODebugContext* dbg) override {
  //   return FileSystemWrapper::LinkFile(s, t, options, dbg);
  // }

  // IOStatus NumFileLinks(const std::string& fname, const IOOptions& options,
  //                       uint64_t* count, IODebugContext* dbg) override {
  //   return FileSystemWrapper::NumFileLinks(fname, options, count, dbg);
  // }

  // IOStatus AreFilesSame(const std::string& first, const std::string& second,
  //                       const IOOptions& options, bool* res,
  //                       IODebugContext* dbg) override {
  //   return FileSystemWrapper::AreFilesSame(first, second, options, res, dbg);
  // }

  // IOStatus LockFile(const std::string& f, const IOOptions& options,
  //                   FileLock** l, IODebugContext* dbg) override {
  //   return FileSystemWrapper::LockFile(f, options, l, dbg);
  // }

  // IOStatus UnlockFile(FileLock* l, const IOOptions& options,
  //                     IODebugContext* dbg) override {
  //   return FileSystemWrapper::UnlockFile(l, options, dbg);
  // }

  // IOStatus GetTestDirectory(const IOOptions& options, std::string* path,
  //                           IODebugContext* dbg) override {
  //   return FileSystemWrapper::GetTestDirectory(options, path, dbg);
  // }
  // IOStatus NewLogger(const std::string& fname, const IOOptions& options,
  //                    std::shared_ptr<Logger>* result,
  //                    IODebugContext* dbg) override {
  //   return FileSystemWrapper::NewLogger(fname, options, result, dbg);
  // }

  // void SanitizeFileOptions(FileOptions* opts) const override {
  //   FileSystemWrapper::SanitizeFileOptions(opts);
  // }

  // FileOptions OptimizeForLogRead(
  //     const FileOptions& file_options) const override {
  //   return FileSystemWrapper::OptimizeForLogRead(file_options);
  // }
  // FileOptions OptimizeForManifestRead(
  //     const FileOptions& file_options) const override {
  //   return FileSystemWrapper::OptimizeForManifestRead(file_options);
  // }
  // FileOptions OptimizeForLogWrite(const FileOptions& file_options,
  //                                 const DBOptions& db_options) const override {
  //   return FileSystemWrapper::OptimizeForLogWrite(file_options, db_options);
  // }
  // FileOptions OptimizeForManifestWrite(
  //     const FileOptions& file_options) const override {
  //   return FileSystemWrapper::OptimizeForManifestWrite(file_options);
  // }
  // FileOptions OptimizeForCompactionTableWrite(
  //     const FileOptions& file_options,
  //     const ImmutableDBOptions& immutable_ops) const override {
  //   return FileSystemWrapper::OptimizeForCompactionTableWrite(file_options,
  //                                                   immutable_ops);
  // }
  // FileOptions OptimizeForCompactionTableRead(
  //     const FileOptions& file_options,
  //     const ImmutableDBOptions& db_options) const override {
  //   return FileSystemWrapper::OptimizeForCompactionTableRead(file_options, db_options);
  // }
  // FileOptions OptimizeForBlobFileRead(
  //     const FileOptions& file_options,
  //     const ImmutableDBOptions& db_options) const override {
  //   return FileSystemWrapper::OptimizeForBlobFileRead(file_options, db_options);
  // }
  // IOStatus GetFreeSpace(const std::string& path, const IOOptions& options,
  //                       uint64_t* diskfree, IODebugContext* dbg) override {
  //   return FileSystemWrapper::GetFreeSpace(path, options, diskfree, dbg);
  // }
  // IOStatus IsDirectory(const std::string& path, const IOOptions& options,
  //                      bool* is_dir, IODebugContext* dbg) override {
  //   return FileSystemWrapper::IsDirectory(path, options, is_dir, dbg);
  // }

  // const Customizable* Inner() const override { return target_.get(); }
  // Status PrepareOptions(const ConfigOptions& options) override;
  // std::string SerializeOptions(const ConfigOptions& config_options,
  //                              const std::string& header) const override;

  // IOStatus Poll(std::vector<void*>& io_handles,
  //               size_t min_completions) override {
  //   return FileSystemWrapper::Poll(io_handles, min_completions);
  // }

  // IOStatus AbortIO(std::vector<void*>& io_handles) override {
  //   return FileSystemWrapper::AbortIO(io_handles);
  // }

  // void DiscardCacheForDirectory(const std::string& path) override {
  //   FileSystemWrapper::DiscardCacheForDirectory(path);
  // }

  // void SupportedOps(int64_t& supported_ops) override {
  //   return FileSystemWrapper::SupportedOps(supported_ops);
  // }
};

std::unique_ptr<FileSystem>
NewEngineSwapFileSystem() {
  return std::unique_ptr<FileSystem>(
      new EngineSwapFileSystem(FileSystem::Default()));
}

}  // namespace ROCKSDB_NAMESPACE
