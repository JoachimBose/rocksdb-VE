#include "engineswap.h"

#include <iostream>
#include <string>
#include <memory>

#define URI_SEPERATOR "://"
#define URI_POSIX "posix"
#define URI_IOU "io_uring"
#define URI_AIO "libaio"
#define URI_IOUNV "io_uring_nv"
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



 

  std::unique_ptr<AioRing>* EngineSwapFileSystem::getAioRing(){
    if (aioRing.get() == nullptr)
    {
      aioRing.reset(new AioRing());
    }
    
    return &aioRing;
  }
    
  std::unique_ptr<IouRing>* EngineSwapFileSystem::getRing(){
    if (ring.get() == nullptr)
    {
      ring.reset(new IouRing());
    }
    
    return &ring;
  }
  
  EngineSwapFileSystem::EngineSwapFileSystem(std::shared_ptr<FileSystem> t, std::string nengine_type) : FileSystemWrapper(t) {
      
    engine_type = nengine_type;
    
    #define setEngineType(x) \
    NewRandomAccessFileStub = NewRandomAccessFile##x; \
    NewWritableFileStub = NewWritableFile##x; \
    NewSequentialFileStub = NewSequentialFile##x;
    //set up which fs to use
    if(nengine_type.compare(URI_POSIX) == 0){
      setEngineType(Posix);
    }
    else if (nengine_type.compare(URI_IOU) == 0){
      setEngineType(Iou)
      std::cout << "Selected api is IO_URING, the fan favorite backend :) \n";
    }
    else if (nengine_type.compare(URI_IOUNV) == 0){
      setEngineType(IouNv)
      std::cout << "Selected api is IO_URING NO VECTORISER, the highly anticipated (non)sequel :) \n";
    }
    else if (nengine_type.compare(URI_AIO) == 0){
      setEngineType(Aio)
      std::cout << "Selected api is libaio but is not yet implemented :(\n";
    }
    else if (nengine_type.compare(URI_DUMMY) == 0){
      std::cout << "Selected api is dummy\n";
    }
    #undef setEngineType
  }

  const char* EngineSwapFileSystem::Name() const { return "EngineFileSystem"; }
  
  IOStatus EngineSwapFileSystem::NewSequentialFile(const std::string& f, const FileOptions& file_opts,
                             std::unique_ptr<FSSequentialFile>* r,
                             IODebugContext* dbg) {
    
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
  IOStatus EngineSwapFileSystem::NewRandomAccessFile(const std::string& f,
                               const FileOptions& file_opts,
                               std::unique_ptr<FSRandomAccessFile>* r,
                               IODebugContext* dbg) {
    
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
  IOStatus EngineSwapFileSystem::NewWritableFile(const std::string& f,
                           const FileOptions& file_opts,
                           std::unique_ptr<FSWritableFile>* r,
                           IODebugContext* dbg) {
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

std::unique_ptr<FileSystem>
NewEngineSwapFileSystem(std::string engine_type) {
  std::unique_ptr<FileSystem> fs = std::unique_ptr<FileSystem>(
    std::unique_ptr<FileSystem>(new EngineSwapFileSystem(FileSystem::Default(), engine_type)));
    std::cout << "creating new engine with type: " << engine_type << "\n";
  return fs;

}

}  // namespace ROCKSDB_NAMESPACE