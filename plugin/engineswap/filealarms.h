#pragma once

#ifndef FILEALARM_H
#define FILEALARM_H

#include "rocksdb/file_system.h"
#include "rocksdb/io_status.h"

#include <iostream>
#include <string>
#include <fstream>

#define ALARM_FILE_DUMMY "~/rocksdb-alerts.txt"

namespace rocksdb{

void raiseAlarm(std::string alarm);

//#region some wrappers for alarming when unimplemented functions have been called

class WritableFileAlarm : public FSWritableFileWrapper {
 public:

  WritableFileAlarm(FSWritableFile* base) : FSWritableFileWrapper(base){

  }
  // IOStatus Append(const Slice& data, const IOOptions& options,
  //                 IODebugContext* dbg) override {
  //   raiseAlarm("WritableFileAlarm::Append2 \n");
  //   return FSWritableFileWrapper::Append(data, options, dbg);
  // }
  IOStatus Append(const Slice& data, const IOOptions& options,
                  const DataVerificationInfo& verification_info,
                  IODebugContext* dbg) override {
    raiseAlarm("WritableFileAlarm::Append2 \n");
    return FSWritableFileWrapper::Append(data, options, verification_info, dbg);
  }
  IOStatus PositionedAppend(const Slice& data, uint64_t offset,
                            const IOOptions& options,
                            IODebugContext* dbg) override {
    raiseAlarm("WritableFileAlarm::PositionedAppend \n");
    return FSWritableFileWrapper::PositionedAppend(data, offset, options, dbg);
  }
  IOStatus PositionedAppend(const Slice& data, uint64_t offset,
                            const IOOptions& options,
                            const DataVerificationInfo& verification_info,
                            IODebugContext* dbg) override {
    raiseAlarm("WritableFileAlarm::PositionedAppend \n");
    return FSWritableFileWrapper::PositionedAppend(data, offset, options, verification_info,
                                     dbg);
  }
};


class RandomAccessFileAlarm : public FSRandomAccessFileWrapper {
 public:
  // Creates a FileWrapper around the input File object and without
  // taking ownership of the object

  RandomAccessFileAlarm(FSRandomAccessFile* base) : FSRandomAccessFileWrapper(base){

  }
  
  IOStatus ReadAsync(FSReadRequest& req, const IOOptions& opts,
                     std::function<void(FSReadRequest&, void*)> cb,
                     void* cb_arg, void** io_handle, IOHandleDeleter* del_fn,
                     IODebugContext* dbg) override {
    raiseAlarm("-> \n");
    return target()->ReadAsync(req, opts, cb, cb_arg, io_handle, del_fn, dbg);
  }
};

class SequentialFileAlarm : public FSSequentialFileWrapper {
 public:
  // Creates a FileWrapper around the input File object and without
  // taking ownership of the object

  SequentialFileAlarm(FSSequentialFile* base) : FSSequentialFileWrapper(base){

  }

  IOStatus Skip(uint64_t n) override { 
    raiseAlarm("SequentialFileAlarm::Skip \n");
    return FSSequentialFileWrapper::Skip(n); }
  
  IOStatus PositionedRead(uint64_t offset, size_t n, const IOOptions& options,
                          Slice* result, char* scratch,
                          IODebugContext* dbg) override {
    raiseAlarm("SequentialFileAlarm::PositionedRead \n");
    return FSSequentialFileWrapper::PositionedRead(offset, n, options, result, scratch, dbg);
  }
};

class FileSystemAlarm : public FileSystemWrapper{
   public:
  // Initialize an EnvWrapper that delegates all calls to *t
  explicit FileSystemAlarm(const std::shared_ptr<FileSystem>& t) : FileSystemWrapper(t){

  }
  
  IOStatus ReopenWritableFile(const std::string& fname,
                              const FileOptions& file_opts,
                              std::unique_ptr<FSWritableFile>* result,
                              IODebugContext* dbg) override {
    raiseAlarm("FileSystemAlarm::ReopenWritableFile\n");
    return FileSystemWrapper::ReopenWritableFile(fname, file_opts, result, dbg);
  }
  IOStatus ReuseWritableFile(const std::string& fname,
                             const std::string& old_fname,
                             const FileOptions& file_opts,
                             std::unique_ptr<FSWritableFile>* r,
                             IODebugContext* dbg) override {
    raiseAlarm("FileSystemAlarm::ReuseWritableFile\n");
    return FileSystemWrapper::ReuseWritableFile(fname, old_fname, file_opts, r, dbg);
  }
  IOStatus NewRandomRWFile(const std::string& fname,
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomRWFile>* result,
                           IODebugContext* dbg) override {
    raiseAlarm("FileSystemAlarm::NewRandomRWFile\n");
    return FileSystemWrapper::NewRandomRWFile(fname, file_opts, result, dbg);
  }
  IOStatus NewMemoryMappedFileBuffer(
      const std::string& fname,
      std::unique_ptr<MemoryMappedFileBuffer>* result) override {
    raiseAlarm("FileSystemAlarm::NewMemoryMappedFileBuffer\n");
    return FileSystemWrapper::NewMemoryMappedFileBuffer(fname, result);
  }
};

//#endregion
}
#endif