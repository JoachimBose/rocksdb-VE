#include "engineswap.h"
#include "iouengine.h"

#include <rocksdb/utilities/object_registry.h>
#include <iostream>
#include <string>

#define URI_SEPERATOR "://"
#define URI_POSIX "posix"
#define URI_IOU "io_uring"
#define URI_AIO "libaio"

namespace ROCKSDB_NAMESPACE {

extern "C" FactoryFunc<FileSystem> engineswap_reg;

FactoryFunc<FileSystem> engineswap_reg =
    ObjectLibrary::Default()->AddFactory<FileSystem>(
        ObjectLibrary::PatternEntry("engineswap", false)
            .AddSeparator(URI_SEPERATOR),
        [](const std::string& uri, std::unique_ptr<FileSystem>* f,
           std::string* /* errmsg */) {
          
          int type_start = uri.find(URI_SEPERATOR) + 3;
          std::string engine_type = uri.substr(type_start);
          if(engine_type.compare(URI_POSIX) == 0){
            std::cout << "Selected api is posix but is not yet implemented :(\n";
          }
          else if (engine_type.compare(URI_IOU) == 0){
            std::cout << "Selected api is io_uring but is not yet implemented :(\n";
          }
          else if (engine_type.compare(URI_AIO) == 0){
            std::cout << "Selected api is libaio but is not yet implemented :(\n";
          }

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
  IOStatus NewDirectory(const std::string& name, const IOOptions& io_opts,
                        std::unique_ptr<FSDirectory>* result,
                        IODebugContext* dbg) override {
    std::cout << " NewDirectory\n";
    return FileSystemWrapper::NewDirectory(name, io_opts, result, dbg);
  }
  IOStatus FileExists(const std::string& f, const IOOptions& io_opts,
                      IODebugContext* dbg) override {
    std::cout << " FileExists\n";
    return FileSystemWrapper::FileExists(f, io_opts, dbg);
  }
  IOStatus GetChildren(const std::string& dir, const IOOptions& io_opts,
                       std::vector<std::string>* r,
                       IODebugContext* dbg) override {
    std::cout << " GetChildren\n";
    return FileSystemWrapper::GetChildren(dir, io_opts, r, dbg);
  }
  IOStatus GetChildrenFileAttributes(const std::string& dir,
                                     const IOOptions& options,
                                     std::vector<FileAttributes>* result,
                                     IODebugContext* dbg) override {
    std::cout << " GetChildrenFileAttributes\n";
    return FileSystemWrapper::GetChildrenFileAttributes(dir, options, result, dbg);
  }
  IOStatus DeleteFile(const std::string& f, const IOOptions& options,
                      IODebugContext* dbg) override {
    std::cout << " DeleteFile\n";
    return FileSystemWrapper::DeleteFile(f, options, dbg);
  }
  IOStatus Truncate(const std::string& fname, size_t size,
                    const IOOptions& options, IODebugContext* dbg) override {
    std::cout << " Truncate\n";
    return FileSystemWrapper::Truncate(fname, size, options, dbg);
  }
  IOStatus CreateDir(const std::string& d, const IOOptions& options,
                     IODebugContext* dbg) override {
    std::cout << " CreateDir\n";
    return FileSystemWrapper::CreateDir(d, options, dbg);
  }
  IOStatus CreateDirIfMissing(const std::string& d, const IOOptions& options,
                              IODebugContext* dbg) override {
    std::cout << " CreateDirIfMissing\n";
    return FileSystemWrapper::CreateDirIfMissing(d, options, dbg);
  }
  IOStatus DeleteDir(const std::string& d, const IOOptions& options,
                     IODebugContext* dbg) override {
    std::cout << " DeleteDir\n";
    return FileSystemWrapper::DeleteDir(d, options, dbg);
  }
  IOStatus GetFileSize(const std::string& f, const IOOptions& options,
                       uint64_t* s, IODebugContext* dbg) override {
    std::cout << " GetFileSize\n";
    return FileSystemWrapper::GetFileSize(f, options, s, dbg);
  }

  IOStatus GetFileModificationTime(const std::string& fname,
                                   const IOOptions& options,
                                   uint64_t* file_mtime,
                                   IODebugContext* dbg) override {
    std::cout << " GetFileModificationTime\n";
    return FileSystemWrapper::GetFileModificationTime(fname, options, file_mtime, dbg);
  }

  IOStatus GetAbsolutePath(const std::string& db_path, const IOOptions& options,
                           std::string* output_path,
                           IODebugContext* dbg) override {
    std::cout << " GetAbsolutePath\n";
    return FileSystemWrapper::GetAbsolutePath(db_path, options, output_path, dbg);
  }

  IOStatus RenameFile(const std::string& s, const std::string& t,
                      const IOOptions& options, IODebugContext* dbg) override {
    std::cout << " RenameFile\n";
    return FileSystemWrapper::RenameFile(s, t, options, dbg);
  }

  IOStatus LinkFile(const std::string& s, const std::string& t,
                    const IOOptions& options, IODebugContext* dbg) override {
    std::cout << " LinkFile\n";
    return FileSystemWrapper::LinkFile(s, t, options, dbg);
  }

  IOStatus NumFileLinks(const std::string& fname, const IOOptions& options,
                        uint64_t* count, IODebugContext* dbg) override {
    std::cout << " NumFileLinks\n";
    return FileSystemWrapper::NumFileLinks(fname, options, count, dbg);
  }

  IOStatus AreFilesSame(const std::string& first, const std::string& second,
                        const IOOptions& options, bool* res,
                        IODebugContext* dbg) override {
    std::cout << " AreFilesSame\n";
    return FileSystemWrapper::AreFilesSame(first, second, options, res, dbg);
  }

  IOStatus LockFile(const std::string& f, const IOOptions& options,
                    FileLock** l, IODebugContext* dbg) override {
    std::cout << " LockFile\n";
    return FileSystemWrapper::LockFile(f, options, l, dbg);
  }

  IOStatus UnlockFile(FileLock* l, const IOOptions& options,
                      IODebugContext* dbg) override {
    std::cout << " UnlockFile\n";
    return FileSystemWrapper::UnlockFile(l, options, dbg);
  }

  IOStatus GetTestDirectory(const IOOptions& options, std::string* path,
                            IODebugContext* dbg) override {
    std::cout << " GetTestDirectory\n";
    return FileSystemWrapper::GetTestDirectory(options, path, dbg);
  }
  IOStatus NewLogger(const std::string& fname, const IOOptions& options,
                     std::shared_ptr<Logger>* result,
                     IODebugContext* dbg) override {
    std::cout << " NewLogger\n";
    return FileSystemWrapper::NewLogger(fname, options, result, dbg);
  }

  void SanitizeFileOptions(FileOptions* opts) const override {
    std::cout << " SanitizeFileOptions\n";
    FileSystemWrapper::SanitizeFileOptions(opts);
  }

  FileOptions OptimizeForLogRead(
      const FileOptions& file_options) const override {
    std::cout << " OptimizeForLogRead\n";
    return FileSystemWrapper::OptimizeForLogRead(file_options);
  }
  FileOptions OptimizeForManifestRead(
      const FileOptions& file_options) const override {
      std::cout << " OptimizeForManifestRead\n";
    return FileSystemWrapper::OptimizeForManifestRead(file_options);
  }
  FileOptions OptimizeForLogWrite(const FileOptions& file_options,
                                  const DBOptions& db_options) const override {
    std::cout << " OptimizeForLogWrite\n";
    return FileSystemWrapper::OptimizeForLogWrite(file_options, db_options);
  }
  FileOptions OptimizeForManifestWrite(
      const FileOptions& file_options) const override {
    std::cout << " OptimizeForManifestWrite\n";
    return FileSystemWrapper::OptimizeForManifestWrite(file_options);
  }
  FileOptions OptimizeForCompactionTableWrite(
      const FileOptions& file_options,
      const ImmutableDBOptions& immutable_ops) const override {
      std::cout << " OptimizeForCompactionTableWrite\n";
    return FileSystemWrapper::OptimizeForCompactionTableWrite(file_options,
                                                    immutable_ops);
  }
  FileOptions OptimizeForCompactionTableRead(
      const FileOptions& file_options,
      const ImmutableDBOptions& db_options) const override {
      std::cout << " OptimizeForCompactionTableRead\n";
    return FileSystemWrapper::OptimizeForCompactionTableRead(file_options, db_options);
  }
  FileOptions OptimizeForBlobFileRead(
      const FileOptions& file_options,
      const ImmutableDBOptions& db_options) const override {
      std::cout << " OptimizeForBlobFileRead\n";
    return FileSystemWrapper::OptimizeForBlobFileRead(file_options, db_options);
  }
  IOStatus GetFreeSpace(const std::string& path, const IOOptions& options,
                        uint64_t* diskfree, IODebugContext* dbg) override {
    std::cout << " GetFreeSpace\n";
    return FileSystemWrapper::GetFreeSpace(path, options, diskfree, dbg);
  }
  IOStatus IsDirectory(const std::string& path, const IOOptions& options,
                       bool* is_dir, IODebugContext* dbg) override {
    std::cout << " IsDirectory\n";
    return FileSystemWrapper::IsDirectory(path, options, is_dir, dbg);
  }

  const Customizable* Inner() const override { 
    std::cout << " Inner\n";
    return target_.get(); 
  }

  IOStatus Poll(std::vector<void*>& io_handles,
                size_t min_completions) override {
    std::cout << " Poll\n";
    return FileSystemWrapper::Poll(io_handles, min_completions);
  }

  IOStatus AbortIO(std::vector<void*>& io_handles) override {
    std::cout << " AbortIO\n";
    return FileSystemWrapper::AbortIO(io_handles);
  }

  void DiscardCacheForDirectory(const std::string& path) override {
    std::cout << " DiscardCacheForDirectory\n";
    FileSystemWrapper::DiscardCacheForDirectory(path);
  }

  void SupportedOps(int64_t& supported_ops) override {
    // std::cout << " SupportedOps\n";
    return FileSystemWrapper::SupportedOps(supported_ops);
  }
};

std::unique_ptr<FileSystem>
NewEngineSwapFileSystem() {
  return std::unique_ptr<FileSystem>(
      new EngineSwapFileSystem(FileSystem::Default()));
}

}  // namespace ROCKSDB_NAMESPACE
