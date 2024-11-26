#ifndef ENGINESWAP_POSIX
#define ENGINESWAP_POSIX

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"

namespace rocksdb{
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
                        IODebugContext* dbg) const override
    {

    }

    RandomAccessFilePosix(const std::string& fname, FILE* file, int fd,
                      size_t logical_block_size, const EnvOptions& options)
    {

    }
    

  // Readahead the file starting from offset by n bytes for caching.
  // If it's not implemented (default: `NotSupported`), RocksDB will create
  // internal prefetch buffer to improve read performance.
    IOStatus Prefetch(uint64_t /*offset*/, size_t /*n*/,
                            const IOOptions& /*options*/,
                            IODebugContext* /*dbg*/) {
    return IOStatus::NotSupported("Prefetch");
  }

  // it should return after all reads have completed. The reads will be
  // non-overlapping but can be in any order. The function return status
  // is only meant for errors that occur before processing individual read
  // requests.
  IOStatus MultiRead(FSReadRequest* reqs, size_t num_reqs,
                             const IOOptions& options, IODebugContext* dbg) {
    assert(reqs != nullptr);
    for (size_t i = 0; i < num_reqs; ++i) {
      FSReadRequest& req = reqs[i];
      req.status =
          Read(req.offset, req.len, options, &req.result, req.scratch, dbg);
    }
    return IOStatus::OK();
  }

  // Tries to get an unique ID for this file that will be the same each time
  // the file is opened (and will stay the same while the file is open).
  // Furthermore, it tries to make this ID at most "max_size" bytes. If such an
  // ID can be created this function returns the length of the ID and places it
  // in "id"; otherwise, this function returns 0, in which case "id"
  // may not have been modified.
  //
  // This function guarantees, for IDs from a given environment, two unique ids
  // cannot be made equal to each other by adding arbitrary bytes to one of
  // them. That is, no unique ID is the prefix of another.
  //
  // This function guarantees that the returned ID will not be interpretable as
  // a single varint.
  //
  // Note: these IDs are only valid for the duration of the process.
  size_t GetUniqueId(char* /*id*/, size_t /*max_size*/) const {
    return 0;  // Default implementation to prevent issues with backwards
               // compatibility.
  }

  bool use_direct_io() const { return false; }

  size_t GetRequiredBufferAlignment() const { return kDefaultPageSize; }

  IOStatus InvalidateCache(size_t /*offset*/, size_t /*length*/) {
    return IOStatus::NotSupported("InvalidateCache not supported.");
  }

  IOStatus ReadAsync(FSReadRequest& req, const IOOptions& opts,
                             std::function<void(FSReadRequest&, void*)> cb,
                             void* cb_arg, void** /*io_handle*/,
                             IOHandleDeleter* /*del_fn*/, IODebugContext* dbg) {
    req.status =
        Read(req.offset, req.len, opts, &(req.result), req.scratch, dbg);
    cb(req, cb_arg);
    return IOStatus::OK();
  }
};

class SequentialFilePosix : public FSSequentialFile {
 private:
  std::string filename;
  FILE* file;
  int fd;
  size_t logical_sector_size;

 public:
  SequentialFilePosix(const std::string& fname, FILE* file, int fd,
                      size_t logical_block_size, const EnvOptions& options);
  ~SequentialFilePosix();

  IOStatus Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
                IODebugContext* dbg) override;
  IOStatus PositionedRead(uint64_t offset, size_t n, const IOOptions& opts,
                          Slice* result, char* scratch,
                          IODebugContext* dbg) override;
  IOStatus Skip(uint64_t n) override;
  IOStatus InvalidateCache(size_t offset, size_t length) override;
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

 public:
  WritableFilePosix(const std::string& fname, FILE* file, int fd,
                      size_t logical_block_size, const EnvOptions& options)
  {
    
  }

  inline ~WritableFilePosix(){
    
  }

  IOStatus Append(const Slice& data, const IOOptions& options,
                          IODebugContext* dbg) = 0;

  IOStatus Append(const Slice& data, const IOOptions& options,
                          const DataVerificationInfo& /* verification_info */,
                          IODebugContext* dbg) {
    return Append(data, options, dbg);
  }

   IOStatus PositionedAppend(const Slice& /* data */,
                                    uint64_t /* offset */,
                                    const IOOptions& /*options*/,
                                    IODebugContext* /*dbg*/) {
    return IOStatus::NotSupported("PositionedAppend");
  }

  IOStatus PositionedAppend(
      const Slice& /* data */, uint64_t /* offset */,
      const IOOptions& /*options*/,
      const DataVerificationInfo& /* verification_info */,
      IODebugContext* /*dbg*/) {
    return IOStatus::NotSupported("PositionedAppend");
  }

  IOStatus Truncate(uint64_t /*size*/, const IOOptions& /*options*/,
                            IODebugContext* /*dbg*/) {
    return IOStatus::OK();
  }

  IOStatus Close(const IOOptions& /*options*/,
                         IODebugContext* /*dbg*/) = 0;

  IOStatus Flush(const IOOptions& options, IODebugContext* dbg) = 0;
  IOStatus Sync(const IOOptions& options,
                        IODebugContext* dbg) = 0;  // sync data

  IOStatus Fsync(const IOOptions& options, IODebugContext* dbg) {
    return Sync(options, dbg);
  }

  // true if Sync() and Fsync() are safe to call concurrently with Append()
  // and Flush().
  bool IsSyncThreadSafe() const { return false; }

  // Indicates the upper layers if the current WritableFile implementation
  // uses direct IO.
  bool use_direct_io() const { return false; }

  // Use the returned alignment value to allocate
  // aligned buffer for Direct I/O
  size_t GetRequiredBufferAlignment() const { return kDefaultPageSize; }

  void SetWriteLifeTimeHint(Env::WriteLifeTimeHint hint) {
    write_hint_ = hint;
  }

  /*
   * If rate limiting is enabled, change the file-granularity priority used in
   * rate-limiting writes.
   *
   * In the presence of finer-granularity priority such as
   * `WriteOptions::rate_limiter_priority`, this file-granularity priority may
   * be overridden by a non-Env::IO_TOTAL finer-granularity priority and used as
   * a fallback for Env::IO_TOTAL finer-granularity priority.
   *
   * If rate limiting is not enabled, this call has no effect.
   */
  void SetIOPriority(Env::IOPriority pri) { io_priority_ = pri; }

  Env::IOPriority GetIOPriority() { return io_priority_; }

  Env::WriteLifeTimeHint GetWriteLifeTimeHint() { return write_hint_; }
  /*
   * Get the size of valid data in the file.
   */
  uint64_t GetFileSize(const IOOptions& /*options*/,
                               IODebugContext* /*dbg*/) = 0;

  /*
   * Get and set the default pre-allocation block size for writes to
   * this file.  If non-zero, then Allocate will be used to extend the
   * underlying storage of a file (generally via fallocate) if the Env
   * instance supports it.
   */
  void SetPreallocationBlockSize(size_t size) {
    preallocation_block_size_ = size;
  }

  void GetPreallocationStatus(size_t* block_size,
                                      size_t* last_allocated_block) {
    *last_allocated_block = last_preallocated_block_;
    *block_size = preallocation_block_size_;
  }

  // For documentation, refer to RandomAccessFile::GetUniqueId()
  size_t GetUniqueId(char* /*id*/, size_t /*max_size*/) const {
    return 0;  // Default implementation to prevent issues with backwards
  }

  // Remove any kind of caching of data from the offset to offset+length
  // of this file. If the length is 0, then it refers to the end of file.
  // If the system is not caching the file contents, then this is a noop.
  // This call has no effect on dirty pages in the cache.
  IOStatus InvalidateCache(size_t /*offset*/, size_t /*length*/) {
    return IOStatus::NotSupported("InvalidateCache not supported.");
  }

  // Sync a file range with disk.
  // offset is the starting byte of the file range to be synchronized.
  // nbytes specifies the length of the range to be synchronized.
  // This asks the OS to initiate flushing the cached data to disk,
  // without waiting for completion.
  // Default implementation does nothing.
  IOStatus RangeSync(uint64_t /*offset*/, uint64_t /*nbytes*/,
                             const IOOptions& options, IODebugContext* dbg) {
    if (strict_bytes_per_sync_) {
      return Sync(options, dbg);
    }
    return IOStatus::OK();
  }

  // PrepareWrite performs any necessary preparation for a write
  // before the write actually occurs.  This allows for pre-allocation
  // of space on devices where it can result in less file
  // fragmentation and/or less waste from over-zealous filesystem
  // pre-allocation.
  void PrepareWrite(size_t offset, size_t len, const IOOptions& options,
                            IODebugContext* dbg) {
    if (preallocation_block_size_ == 0) {
      return;
    }
    // If this write would cross one or more preallocation blocks,
    // determine what the last preallocation block necessary to
    // cover this write would be and Allocate to that point.
    const auto block_size = preallocation_block_size_;
    size_t new_last_preallocated_block =
        (offset + len + block_size - 1) / block_size;
    if (new_last_preallocated_block > last_preallocated_block_) {
      size_t num_spanned_blocks =
          new_last_preallocated_block - last_preallocated_block_;
      Allocate(block_size * last_preallocated_block_,
               block_size * num_spanned_blocks, options, dbg)
          .PermitUncheckedError();
      last_preallocated_block_ = new_last_preallocated_block;
    }
  }

  // Pre-allocates space for a file.
  IOStatus Allocate(uint64_t /*offset*/, uint64_t /*len*/,
                            const IOOptions& /*options*/,
                            IODebugContext* /*dbg*/) {
    return IOStatus::OK();
  }

  // If you're adding methods here, remember to add them to
  // WritableFileWrapper too.

 protected:
  size_t preallocation_block_size() { return preallocation_block_size_; }

 private:
  size_t last_preallocated_block_;
  size_t preallocation_block_size_;
  // No copying allowed
  // FSWritableFile(const FSWritableFile&);
  void operator=(const FSWritableFile&);

 protected:
  Env::IOPriority io_priority_;
  Env::WriteLifeTimeHint write_hint_;
  const bool strict_bytes_per_sync_ = false;
};

}

#endif
