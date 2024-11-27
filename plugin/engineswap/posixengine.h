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

class RandomAccessFilePosix : public FSRandomAccessFile {
 private:
  std::string filename;
  FILE* file;
  int fd;
  size_t logical_sector_size;

 public:
  IOStatus Read(uint64_t offset, size_t n, const IOOptions& options,
                Slice* result, char* scratch,
                IODebugContext* dbg) const override;

  RandomAccessFilePosix(const std::string& fname, FILE* nfile, int nfd,
                        size_t nlogical_block_size, const EnvOptions& noptions)
    {
        filename = fname;
        file = nfile;
        fd = nfd;
        logical_sector_size = nlogical_block_size;
    }

  // Readahead the file starting from offset by n bytes for caching.
  // If it's not implemented (default: `NotSupported`), RocksDB will create
  // internal prefetch buffer to improve read performance.
  IOStatus Prefetch(uint64_t /*offset*/, size_t /*n*/,
                    const IOOptions& /*options*/, IODebugContext* /*dbg*/) {
    return IOStatus::NotSupported("Prefetch");
  }

  // #region nonNeccessaryFunctions
  IOStatus MultiRead(FSReadRequest* reqs, size_t num_reqs,
                     const IOOptions& options, IODebugContext* dbg) {
    return FSRandomAccessFile::MultiRead(reqs, num_reqs, options, dbg);
  }

  size_t GetUniqueId(char* /*id*/, size_t /*max_size*/) const { return 0; }

  bool use_direct_io() const { return false; }

  size_t GetRequiredBufferAlignment() const { return kDefaultPageSize; }

  IOStatus InvalidateCache(size_t /*offset*/, size_t /*length*/) {
    return IOStatus::NotSupported("InvalidateCache not supported.");
  }

  IOStatus ReadAsync(FSReadRequest& req, const IOOptions& opts,
                     std::function<void(FSReadRequest&, void*)> cb,
                     void* cb_arg, void** /*io_handle*/,
                     IOHandleDeleter* /*del_fn*/, IODebugContext* dbg) {
    return FSRandomAccessFile::ReadAsync(req, opts, cb, cb_arg, NULL, NULL,
                                         dbg);
  }
#//endregion
};

class SequentialFilePosix : public FSSequentialFile {
 private:
  std::string filename;
  FILE* file;
  int fd;
  size_t logical_sector_size;

 public:
  SequentialFilePosix(const std::string& fname, FILE* nfile, int nfd,
                      size_t nlogical_block_size, const EnvOptions& noptions)
  {
      filename = fname;
      file = nfile;
      fd = nfd;
      logical_sector_size = nlogical_block_size;
  }
  ~SequentialFilePosix() {}

  IOStatus Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
                IODebugContext* dbg) override;
  IOStatus PositionedRead(uint64_t offset, size_t n, const IOOptions& opts,
                          Slice* result, char* scratch,
                          IODebugContext* dbg) override {
    return IOStatus::NotSupported(
        "SequentialFilePosix::Skip() is not implemented");
  }

  IOStatus Skip(uint64_t n) override {
    return IOStatus::NotSupported(
        "SequentialFilePosix::Skip() is not implemented");
  }

  IOStatus InvalidateCache(size_t offset, size_t length) override{

  }

  bool use_direct_io() const override { false; }

  size_t GetRequiredBufferAlignment() const override {
    return logical_sector_size;
  }
};

class WritableFilePosix : public FSWritableFile {
 private:
  std::string filename;
  FILE* file;
  int fd;
  size_t logical_sector_size;
  size_t filesize;

 public:
  WritableFilePosix(const std::string& fname, FILE* nfile, int nfd,
                    size_t nlogical_block_size, const EnvOptions& noptions) {
        filename = fname;
        file = nfile;
        fd = nfd;
        logical_sector_size = nlogical_block_size;
        filesize = 0;
    }

  inline ~WritableFilePosix() {}

  IOStatus Append(const Slice& data, const IOOptions& options,
                  IODebugContext* dbg);

  IOStatus Append(const Slice& data, const IOOptions& options,
                  const DataVerificationInfo& /* verification_info */,
                  IODebugContext* dbg) {
    return Append(data, options, dbg);
  }

  IOStatus PositionedAppend(const Slice& /* data */, uint64_t /* offset */,
                            const IOOptions& /*options*/,
                            IODebugContext* /*dbg*/) {
    return IOStatus::NotSupported("PositionedAppend");
  }

  IOStatus PositionedAppend(const Slice& /* data */, uint64_t /* offset */,
                            const IOOptions& /*options*/,
                            const DataVerificationInfo& /* verification_info */,
                            IODebugContext* /*dbg*/) {
    return IOStatus::NotSupported("PositionedAppend");
  }

  IOStatus Truncate(uint64_t /*size*/, const IOOptions& /*options*/,
                    IODebugContext* /*dbg*/) {
    return IOStatus::OK();
  }

  IOStatus Close(const IOOptions& /*options*/, IODebugContext* /*dbg*/);

  IOStatus Flush(const IOOptions& options, IODebugContext* dbg);
  IOStatus Sync(const IOOptions& options,
                IODebugContext* dbg);  // sync data

  IOStatus Fsync(const IOOptions& options, IODebugContext* dbg);

  bool IsSyncThreadSafe() const { return false; }

  bool use_direct_io() const { return false; }

  // Use the returned alignment value to allocate
  // aligned buffer for Direct I/O
  size_t GetRequiredBufferAlignment() const ;

  void SetWriteLifeTimeHint(Env::WriteLifeTimeHint hint) { write_hint_ = hint; }

  void SetIOPriority(Env::IOPriority pri) { io_priority_ = pri; }

  Env::IOPriority GetIOPriority() { return io_priority_; }

  Env::WriteLifeTimeHint GetWriteLifeTimeHint() { return write_hint_; }

  uint64_t GetFileSize(const IOOptions& /*options*/,
                       IODebugContext* /*dbg*/);

  void SetPreallocationBlockSize(size_t size) ;

  void GetPreallocationStatus(size_t* block_size,
                              size_t* last_allocated_block) {
    FSWritableFile::GetPreallocationStatus(block_size, last_allocated_block);
  }
  size_t GetUniqueId(char* /*id*/, size_t /*max_size*/) const {
    return 0;  // Default implementation to prevent issues with backwards
  }
  IOStatus InvalidateCache(size_t /*offset*/, size_t /*length*/) {
    return IOStatus::NotSupported("InvalidateCache not supported.");
  }
  IOStatus RangeSync(uint64_t /*offset*/, uint64_t /*nbytes*/,
                     const IOOptions& options, IODebugContext* dbg);
  void PrepareWrite(size_t offset, size_t len, const IOOptions& options,
                    IODebugContext* dbg) {
    FSWritableFile::PrepareWrite(offset, len, options, dbg);
  }

  // Pre-allocates space for a file.
  IOStatus Allocate(uint64_t /*offset*/, uint64_t /*len*/,
                    const IOOptions& /*options*/, IODebugContext* /*dbg*/) {
    return IOStatus::OK();
  }
};

}
#endif
