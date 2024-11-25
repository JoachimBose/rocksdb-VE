#include "engineswap.h"
#include "iouengine.h"
#include "posixengine.h"
#include "filedummies.h"

#include "rocksdb/file_system.h"
#include "rocksdb/utilities/object_registry.h"

#include <iostream>
#include <string>
#include <memory>

#define URI_SEPERATOR "://"
#define URI_POSIX "posix"
#define URI_IOU "io_uring"
#define URI_AIO "libaio"
#define URI_DUMMY "dummy"

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

          *f = NewEngineSwapFileSystem(engine_type);
          return f->get();
        });


class EngineSwapFileSystem : public FileSystemWrapper {
 private:

  IOStatus (*NewRandomAccessFileStub)(const std::string& f, 
                             const FileOptions& file_opts,
                             std::unique_ptr<FSRandomAccessFile>* r,
                             IODebugContext* dbg);

  IOStatus (*NewSequentialFileStub)(const std::string& f, 
                            const FileOptions& file_opts,
                            std::unique_ptr<FSSequentialFile>* r,
                            IODebugContext* dbg);

  IOStatus (*NewWritableFileStub)(const std::string& f, 
                            const FileOptions& file_opts,
                            std::unique_ptr<FSWritableFile>* r,
                            IODebugContext* dbg);
  std::string engine_type;

 public:
  EngineSwapFileSystem(std::shared_ptr<FileSystem> t, std::string nengine_type) : FileSystemWrapper(t) {
      
    engine_type = nengine_type;
    
    #define setEngineType(x) \
    NewRandomAccessFileStub = NewRandomAccessFile##x; \
    NewWritableFileStub = NewWritableFile##x; \
    NewSequentialFileStub = NewSequentialFile##x;
    //set up which fs to use
    if(nengine_type.compare(URI_POSIX) == 0){
      //setEngineType(Posix);
    }
    else if (nengine_type.compare(URI_IOU) == 0){
      //setEngineType(Iou)
      std::cout << "Selected api is io_uring but is not yet implemented :(\n";
    }
    else if (nengine_type.compare(URI_AIO) == 0){
      //setEngineType(Aio)
      std::cout << "Selected api is libaio but is not yet implemented :(\n";
    }
    else if (nengine_type.compare(URI_DUMMY) == 0){
      std::cout << "Selected api is dummy\n";
    }
    #undef setEngineType
  }

  const char* Name() const override { return "EngineFileSystem"; }
  
  IOStatus NewSequentialFile(const std::string& f, const FileOptions& file_opts,
                             std::unique_ptr<FSSequentialFile>* r,
                             IODebugContext* dbg) override {
    
    std::cout << "creating new sequential file\n";
    if(engine_type.compare(URI_DUMMY) != 0){
      return NewSequentialFileStub(f, file_opts, r, dbg);
    }
    else{
      std::unique_ptr<FSSequentialFile> rcp;
      IOStatus s = FileSystemWrapper::NewSequentialFile(f, file_opts, &rcp, dbg);
      if (s.ok())
      { 
        r->reset(new SequentialFileDummy(rcp.release()));
      }
      return s;
    }
  }
  IOStatus NewRandomAccessFile(const std::string& f,
                               const FileOptions& file_opts,
                               std::unique_ptr<FSRandomAccessFile>* r,
                               IODebugContext* dbg) override {
    
    std::cout << "creating new randomaccess file\n";
    if(engine_type.compare(URI_DUMMY) != 0){
      return NewRandomAccessFileStub(f, file_opts, r, dbg);
    }
    else{
      std::unique_ptr<FSRandomAccessFile> rcp;
      IOStatus s = FileSystemWrapper::NewRandomAccessFile(f, file_opts, &rcp, dbg);
      if (s.ok())
      {
        r->reset(new RandomAccessFileDummy(rcp.release()));
      }
      return s;
    }
  }
  IOStatus NewWritableFile(const std::string& f,
                           const FileOptions& file_opts,
                           std::unique_ptr<FSWritableFile>* r,
                           IODebugContext* dbg) override {
    std::cout << "creating new writeable file\n";
    if(engine_type.compare(URI_DUMMY) != 0){
      return NewWritableFileStub(f, file_opts, r, dbg);
    }
    else{
      std::unique_ptr<FSWritableFile> rcp;
      IOStatus s = FileSystemWrapper::NewWritableFile(f, file_opts, &rcp, dbg);
      if (s.ok())
      {
        r->reset(new WritableFileDummy(rcp.release()));
      }
      else{
        std::cout << "failed \n";
      }
      std::cout << "exiting file creation\n";
      return s;
    }
  }
  };

std::unique_ptr<FileSystem>
NewEngineSwapFileSystem(std::string engine_type) {
  std::unique_ptr<FileSystem> fs = std::unique_ptr<FileSystem>(
    new EngineSwapFileSystem(FileSystem::Default(), engine_type));
    std::cout << "creating new engine with type: " << engine_type << "\n";
  return fs;

}

}  // namespace ROCKSDB_NAMESPACE
