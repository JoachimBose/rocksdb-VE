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
    std::cout << "WritableFileDummy::PositionedAppend \n";
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
    std::cout << "WritableFileDummy::RangeSync \n";
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
  void Hint(AccessPattern pattern) override { FSRandomAccessFileWrapper::Hint(pattern); }
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
    std::cout << "-> \n";
    return target()->ReadAsync(req, opts, cb, cb_arg, io_handle, del_fn, dbg);
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
    std::cout << "SequentialFilSequentialFileDummy::eDummy::Read \n";
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

//#endregion
}
#endif