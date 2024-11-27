#ifndef FILEDUMMIES_H
#define FILEDUMMIES_H

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"

#include <iostream>

namespace rocksdb{

//#region some wrappers for finding the functions required to be overridden

class WritableFileDummy : public FSWritableFileWrapper {
 public:

  WritableFileDummy(FSWritableFile* base) : FSWritableFileWrapper(base){

  }

  IOStatus Append(const Slice& data, const IOOptions& options,
                  IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::Append \n";
    return FSWritableFileWrapper::Append(data, options, dbg);
  }
  IOStatus Append(const Slice& data, const IOOptions& options,
                  const DataVerificationInfo& verification_info,
                  IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::Append2 \n";
    return FSWritableFileWrapper::Append(data, options, verification_info, dbg);
  }
  IOStatus PositionedAppend(const Slice& data, uint64_t offset,
                            const IOOptions& options,
                            IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::PositionedAppend \n";
    return FSWritableFileWrapper::PositionedAppend(data, offset, options, dbg);
  }
  IOStatus PositionedAppend(const Slice& data, uint64_t offset,
                            const IOOptions& options,
                            const DataVerificationInfo& verification_info,
                            IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::PositionedAppend2 \n";
    return FSWritableFileWrapper::PositionedAppend(data, offset, options, verification_info,
                                     dbg);
  }
  IOStatus Truncate(uint64_t size, const IOOptions& options,
                    IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::Truncate \n";
    return FSWritableFileWrapper::Truncate(size, options, dbg);
  }
  IOStatus Close(const IOOptions& options, IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::Close \n";
    return FSWritableFileWrapper::Close(options, dbg);
  }
  IOStatus Flush(const IOOptions& options, IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::Flush \n";
    return FSWritableFileWrapper::Flush(options, dbg);
  }
  IOStatus Sync(const IOOptions& options, IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::Sync \n";
    return FSWritableFileWrapper::Sync(options, dbg);
  }
  IOStatus Fsync(const IOOptions& options, IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::Fsync \n";
    return FSWritableFileWrapper::Fsync(options, dbg);
  }
  bool IsSyncThreadSafe() const override { ;
    std::cout << "WritableFileDummy::IsSyncThreadSafe \n";
    return FSWritableFileWrapper::IsSyncThreadSafe(); }

  bool use_direct_io() const override { ;
    std::cout << "WritableFileDummy::use_direct_io \n";
    return FSWritableFileWrapper::use_direct_io(); }

  size_t GetRequiredBufferAlignment() const override {
    std::cout << "WritableFileDummy::GetRequiredBufferAlignment \n";
    return FSWritableFileWrapper::GetRequiredBufferAlignment();
  }

  void SetWriteLifeTimeHint(Env::WriteLifeTimeHint hint) override {
    std::cout << "WritableFileDummy::SetWriteLifeTimeHint \n";
    FSWritableFileWrapper::SetWriteLifeTimeHint(hint);
  }

  Env::WriteLifeTimeHint GetWriteLifeTimeHint() override {
    std::cout << "WritableFileDummy::GetWriteLifeTimeHint \n";
    return FSWritableFileWrapper::GetWriteLifeTimeHint();
  }

  uint64_t GetFileSize(const IOOptions& options, IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::GetFileSize \n";
    return FSWritableFileWrapper::GetFileSize(options, dbg);
  }

  void SetPreallocationBlockSize(size_t size) override {
    std::cout << "WritableFileDummy::SetPreallocationBlockSize \n";
    FSWritableFileWrapper::SetPreallocationBlockSize(size);
  }

  void GetPreallocationStatus(size_t* block_size,
                              size_t* last_allocated_block) override {
    std::cout << "WritableFileDummy::GetPreallocationStatus \n";
    FSWritableFileWrapper::GetPreallocationStatus(block_size, last_allocated_block);
  }

  size_t GetUniqueId(char* id, size_t max_size) const override {
    std::cout << "WritableFileDummy::GetUniqueId \n";
    return FSWritableFileWrapper::GetUniqueId(id, max_size);
  }

  IOStatus InvalidateCache(size_t offset, size_t length) override {
    std::cout << "WritableFileDummy::InvalidateCache \n";
    return FSWritableFileWrapper::InvalidateCache(offset, length);
  }

  IOStatus RangeSync(uint64_t offset, uint64_t nbytes, const IOOptions& options,
                     IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::RangeSync \n";
    return FSWritableFileWrapper::RangeSync(offset, nbytes, options, dbg);
  }

  void PrepareWrite(size_t offset, size_t len, const IOOptions& options,
                    IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::PrepareWrite \n";
    FSWritableFileWrapper::PrepareWrite(offset, len, options, dbg);
  }

  IOStatus Allocate(uint64_t offset, uint64_t len, const IOOptions& options,
                    IODebugContext* dbg) override {
    std::cout << "WritableFileDummy::Allocate \n";
    return FSWritableFileWrapper::Allocate(offset, len, options, dbg);
  }
};


class RandomAccessFileDummy : public FSRandomAccessFileWrapper {
 public:
  // Creates a FileWrapper around the input File object and without
  // taking ownership of the object

  RandomAccessFileDummy(FSRandomAccessFile* base) : FSRandomAccessFileWrapper(base){

  }

  IOStatus Read(uint64_t offset, size_t n, const IOOptions& options,
                Slice* result, char* scratch,
                IODebugContext* dbg) const override {
    std::cout << "RandomAccessFileDummy::Read \n";
    return FSRandomAccessFileWrapper::Read(offset, n, options, result, scratch, dbg);
  }
  IOStatus MultiRead(FSReadRequest* reqs, size_t num_reqs,
                     const IOOptions& options, IODebugContext* dbg) override {
    std::cout << "RandomAccessFileDummy::MultiRead \n";
    return FSRandomAccessFileWrapper::MultiRead(reqs, num_reqs, options, dbg);
  }
  IOStatus Prefetch(uint64_t offset, size_t n, const IOOptions& options,
                    IODebugContext* dbg) override {
    std::cout << "RandomAccessFileDummy::Prefetch \n";
    return FSRandomAccessFileWrapper::Prefetch(offset, n, options, dbg);
  }
  size_t GetUniqueId(char* id, size_t max_size) const override {
    std::cout << "RandomAccessFileDummy::GetUniqueId \n";
    return FSRandomAccessFileWrapper::GetUniqueId(id, max_size);
  }
  void Hint(AccessPattern pattern) override { 
    std::cout << "RandomAccessFileDummy::Hint\n"
    FSRandomAccessFileWrapper::Hint(pattern);
  }
  bool use_direct_io() const override { 
    std::cout << "RandomAccessFileDummy::use_direct_io \n";
    return FSRandomAccessFileWrapper::use_direct_io(); }
  size_t GetRequiredBufferAlignment() const override {
    std::cout << "RandomAccessFileDummy::GetRequiredBufferAlignment \n";
    return FSRandomAccessFileWrapper::GetRequiredBufferAlignment();
  }
  IOStatus InvalidateCache(size_t offset, size_t length) override {
    std::cout << "RandomAccessFileDummy::InvalidateCache \n";
    return FSRandomAccessFileWrapper::InvalidateCache(offset, length);
  }
  IOStatus ReadAsync(FSReadRequest& req, const IOOptions& opts,
                     std::function<void(FSReadRequest&, void*)> cb,
                     void* cb_arg, void** io_handle, IOHandleDeleter* del_fn,
                     IODebugContext* dbg) override {
    std::cout << "RandomAccessFileDummy::ReadAsync \n";
    return FSRandomAccessFileWrapper::ReadAsync(req, opts, cb, cb_arg, del_fn, dbg);
  }
  Temperature GetTemperature() const override {
    std::cout << "RandomAccessFileDummy::GetTemperature \n";
    return FSRandomAccessFileWrapper::GetTemperature();
  }

};

class SequentialFileDummy : public FSSequentialFileWrapper {
 public:
  // Creates a FileWrapper around the input File object and without
  // taking ownership of the object

  SequentialFileDummy(FSSequentialFile* base) : FSSequentialFileWrapper(base){

  }

  IOStatus Read(size_t n, const IOOptions& options, Slice* result,
                char* scratch, IODebugContext* dbg) override {
    std::cout << "SequentialFileDummy::Read \n";
    return FSSequentialFileWrapper::Read(n, options, result, scratch, dbg);
  }
  IOStatus Skip(uint64_t n) override { 
    std::cout << "SequentialFileDummy::Skip \n";
    return FSSequentialFileWrapper::Skip(n); }
  bool use_direct_io() const override { 
    std::cout << "SequentialFileDummy::use_direct_io \n";
    return FSSequentialFileWrapper::use_direct_io(); }
  size_t GetRequiredBufferAlignment() const override {
    std::cout << "SequentialFileDummy::GetRequiredBufferAlignment \n";
    return FSSequentialFileWrapper::GetRequiredBufferAlignment();
  }
  IOStatus InvalidateCache(size_t offset, size_t length) override {
    std::cout << "SequentialFileDummy::InvalidateCache \n";
    return FSSequentialFileWrapper::InvalidateCache(offset, length);
  }
  IOStatus PositionedRead(uint64_t offset, size_t n, const IOOptions& options,
                          Slice* result, char* scratch,
                          IODebugContext* dbg) override {
    std::cout << "SequentialFileDummy::PositionedRead \n";
    return FSSequentialFileWrapper::PositionedRead(offset, n, options, result, scratch, dbg);
  }
  Temperature GetTemperature() const override {
    std::cout << "SequentialFileDummy::GetTemperature \n";
    return FSSequentialFileWrapper::GetTemperature();
  }
};

class FileSystemDummy : public FileSystemWrapper{
   public:
  // Initialize an EnvWrapper that delegates all calls to *t
  explicit FileSystemDummy(const std::shared_ptr<FileSystem>& t) : FileSystemWrapper(t){

  }
  ~FileSystemDummy() override {}

  const char* Name() const override { return "FileSystemDummy"; }

  // Return the target to which this Env forwards all calls

  // The following text is boilerplate that forwards all methods to target()
  IOStatus NewSequentialFile(const std::string& f, const FileOptions& file_opts,
                             std::unique_ptr<FSSequentialFile>* r,
                             IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::NewSequentialFile\n";
    return FileSystemWrapper::NewSequentialFile(f, file_opts, r, dbg);
  }
  IOStatus NewRandomAccessFile(const std::string& f,
                               const FileOptions& file_opts,
                               std::unique_ptr<FSRandomAccessFile>* r,
                               IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::NewRandomAccessFile\n";
    return FileSystemWrapper::NewRandomAccessFile(f, file_opts, r, dbg);
  }
  IOStatus NewWritableFile(const std::string& f, const FileOptions& file_opts,
                           std::unique_ptr<FSWritableFile>* r,
                           IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::NewWritableFile\n";
    return FileSystemWrapper::NewWritableFile(f, file_opts, r, dbg);
  }
  IOStatus ReopenWritableFile(const std::string& fname,
                              const FileOptions& file_opts,
                              std::unique_ptr<FSWritableFile>* result,
                              IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::ReopenWritableFile\n";
    return FileSystemWrapper::ReopenWritableFile(fname, file_opts, result, dbg);
  }
  IOStatus ReuseWritableFile(const std::string& fname,
                             const std::string& old_fname,
                             const FileOptions& file_opts,
                             std::unique_ptr<FSWritableFile>* r,
                             IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::ReuseWritableFile\n";
    return FileSystemWrapper::ReuseWritableFile(fname, old_fname, file_opts, r, dbg);
  }
  IOStatus NewRandomRWFile(const std::string& fname,
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomRWFile>* result,
                           IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::NewRandomRWFile\n";
    return FileSystemWrapper::NewRandomRWFile(fname, file_opts, result, dbg);
  }
  IOStatus NewMemoryMappedFileBuffer(
      const std::string& fname,
      std::unique_ptr<MemoryMappedFileBuffer>* result) override {
    std::cout << "FileSystemDummy::NewMemoryMappedFileBuffer\n";
    return FileSystemWrapper::NewMemoryMappedFileBuffer(fname, result);
  }
  IOStatus NewDirectory(const std::string& name, const IOOptions& io_opts,
                        std::unique_ptr<FSDirectory>* result,
                        IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::NewDirectory\n";
    return FileSystemWrapper::NewDirectory(name, io_opts, result, dbg);
  }
  IOStatus FileExists(const std::string& f, const IOOptions& io_opts,
                      IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::FileExists\n";
    return FileSystemWrapper::FileExists(f, io_opts, dbg);
  }
  IOStatus GetChildren(const std::string& dir, const IOOptions& io_opts,
                       std::vector<std::string>* r,
                       IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::GetChildren\n";
    return FileSystemWrapper::GetChildren(dir, io_opts, r, dbg);
  }
  IOStatus GetChildrenFileAttributes(const std::string& dir,
                                     const IOOptions& options,
                                     std::vector<FileAttributes>* result,
                                     IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::GetChildrenFileAttributes\n";
    return FileSystemWrapper::GetChildrenFileAttributes(dir, options, result, dbg);
  }
  IOStatus DeleteFile(const std::string& f, const IOOptions& options,
                      IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::DeleteFile\n";
    return FileSystemWrapper::DeleteFile(f, options, dbg);
  }
  IOStatus Truncate(const std::string& fname, size_t size,
                    const IOOptions& options, IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::Truncate\n";
    return FileSystemWrapper::Truncate(fname, size, options, dbg);
  }
  IOStatus CreateDir(const std::string& d, const IOOptions& options,
                     IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::CreateDir\n";
    return FileSystemWrapper::CreateDir(d, options, dbg);
  }
  IOStatus CreateDirIfMissing(const std::string& d, const IOOptions& options,
                              IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::CreateDirIfMissing\n";
    return FileSystemWrapper::CreateDirIfMissing(d, options, dbg);
  }
  IOStatus DeleteDir(const std::string& d, const IOOptions& options,
                     IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::DeleteDir\n";
    return FileSystemWrapper::DeleteDir(d, options, dbg);
  }
  IOStatus GetFileSize(const std::string& f, const IOOptions& options,
                       uint64_t* s, IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::GetFileSize\n";
    return FileSystemWrapper::GetFileSize(f, options, s, dbg);
  }

  IOStatus GetFileModificationTime(const std::string& fname,
                                   const IOOptions& options,
                                   uint64_t* file_mtime,
                                   IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::GetFileModificationTime\n";
    return FileSystemWrapper::GetFileModificationTime(fname, options, file_mtime, dbg);
  }

  IOStatus GetAbsolutePath(const std::string& db_path, const IOOptions& options,
                           std::string* output_path,
                           IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::GetAbsolutePath\n";
    return FileSystemWrapper::GetAbsolutePath(db_path, options, output_path, dbg);
  }

  IOStatus RenameFile(const std::string& s, const std::string& t,
                      const IOOptions& options, IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::RenameFile\n";
    return FileSystemWrapper::RenameFile(s, t, options, dbg);
  }

  IOStatus LinkFile(const std::string& s, const std::string& t,
                    const IOOptions& options, IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::LinkFile\n";
    return FileSystemWrapper::LinkFile(s, t, options, dbg);
  }

  IOStatus NumFileLinks(const std::string& fname, const IOOptions& options,
                        uint64_t* count, IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::NumFileLinks\n";
    return FileSystemWrapper::NumFileLinks(fname, options, count, dbg);
  }

  IOStatus AreFilesSame(const std::string& first, const std::string& second,
                        const IOOptions& options, bool* res,
                        IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::AreFilesSame\n";
    return FileSystemWrapper::AreFilesSame(first, second, options, res, dbg);
  }

  IOStatus LockFile(const std::string& f, const IOOptions& options,
                    FileLock** l, IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::LockFile\n";
    return FileSystemWrapper::LockFile(f, options, l, dbg);
  }

  IOStatus UnlockFile(FileLock* l, const IOOptions& options,
                      IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::UnlockFile\n";
    return FileSystemWrapper::UnlockFile(l, options, dbg);
  }

  IOStatus GetTestDirectory(const IOOptions& options, std::string* path,
                            IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::GetTestDirectory\n";
    return FileSystemWrapper::GetTestDirectory(options, path, dbg);
  }
  IOStatus NewLogger(const std::string& fname, const IOOptions& options,
                     std::shared_ptr<Logger>* result,
                     IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::NewLogger\n";
    return FileSystemWrapper::NewLogger(fname, options, result, dbg);
  }

  void SanitizeFileOptions(FileOptions* opts) const override {
std::cout << "FileSystemDummy::SanitizeFileOptions\n";
    FileSystemWrapper::SanitizeFileOptions(opts);
  }

  FileOptions OptimizeForLogRead(
      const FileOptions& file_options) const override {
    std::cout << "FileSystemDummy::OptimizeForLogRead\n";
    return FileSystemWrapper::OptimizeForLogRead(file_options);
  }
  FileOptions OptimizeForManifestRead(
      const FileOptions& file_options) const override {
    std::cout << "FileSystemDummy::OptimizeForManifestRead\n";
    return FileSystemWrapper::OptimizeForManifestRead(file_options);
  }
  FileOptions OptimizeForLogWrite(const FileOptions& file_options,
                                  const DBOptions& db_options) const override {
    std::cout << "FileSystemDummy::OptimizeForLogWrite\n";
    return FileSystemWrapper::OptimizeForLogWrite(file_options, db_options);
  }
  FileOptions OptimizeForManifestWrite(
      const FileOptions& file_options) const override {
    std::cout << "FileSystemDummy::OptimizeForManifestWrite\n";
    return FileSystemWrapper::OptimizeForManifestWrite(file_options);
  }
  FileOptions OptimizeForCompactionTableWrite(
      const FileOptions& file_options,
      const ImmutableDBOptions& immutable_ops) const override {
    std::cout << "FileSystemDummy::OptimizeForCompactionTableWrite\n";
    return FileSystemWrapper::OptimizeForCompactionTableWrite(file_options,
                                                    immutable_ops);
  }
  FileOptions OptimizeForCompactionTableRead(
      const FileOptions& file_options,
      const ImmutableDBOptions& db_options) const override {
    std::cout << "FileSystemDummy::OptimizeForCompactionTableRead\n";
    return FileSystemWrapper::OptimizeForCompactionTableRead(file_options, db_options);
  }
  FileOptions OptimizeForBlobFileRead(
      const FileOptions& file_options,
      const ImmutableDBOptions& db_options) const override {
    std::cout << "FileSystemDummy::OptimizeForBlobFileRead\n";
    return FileSystemWrapper::OptimizeForBlobFileRead(file_options, db_options);
  }
  IOStatus GetFreeSpace(const std::string& path, const IOOptions& options,
                        uint64_t* diskfree, IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::GetFreeSpace\n";
    return FileSystemWrapper::GetFreeSpace(path, options, diskfree, dbg);
  }
  IOStatus IsDirectory(const std::string& path, const IOOptions& options,
                       bool* is_dir, IODebugContext* dbg) override {
    std::cout << "FileSystemDummy::IsDirectory\n";
    return FileSystemWrapper::IsDirectory(path, options, is_dir, dbg);
  }

  IOStatus Poll(std::vector<void*>& io_handles,
                size_t min_completions) override {
    std::cout << "FileSystemDummy::Poll\n";
    return FileSystemWrapper::Poll(io_handles, min_completions);
  }

  IOStatus AbortIO(std::vector<void*>& io_handles) override {
    std::cout << "FileSystemDummy::AbortIO\n";
    return FileSystemWrapper::AbortIO(io_handles);
  }

  void DiscardCacheForDirectory(const std::string& path) override {
std::cout << "FileSystemDummy::DiscardCacheForDirectory\n";
    FileSystemWrapper::DiscardCacheForDirectory(path);
  }

  void SupportedOps(int64_t& supported_ops) override {
    std::cout << "FileSystemDummy::SupportedOps\n";
    return FileSystemWrapper::SupportedOps(supported_ops);
  }
};

//#endregion
}
#endif