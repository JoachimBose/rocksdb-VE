#include "posixengine.h"
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <sys/stat.h>

namespace rocksdb{
    //uncomment these to make some warnings :)

    size_t getBlkSize(int fd){
        struct stat buf;
        fstat(fd, &buf);
        return buf.st_size;
    }

    int setupFile(const std::string &f, IODebugContext* dbg, mode_t openMode, const char* fopenMode, FILE** fp){
        int flags = O_CLOEXEC | O_CREAT;
        int fd = 0;
        do {
            fd = open(f.c_str(), flags, O_RDWR); 
        } while(fd < 0 && errno == EINTR);
        
        if (fd > 0) {
            *fp = fdopen(fd, fopenMode);
            dbg->msg = ("posixFileError");   
        }
        return fd;
    }

    IOStatus NewRandomAccessFilePosix(const std::string& f, 
                           const FileOptions& file_opts,
                           std::unique_ptr<FSRandomAccessFile>* r,
                           IODebugContext* dbg)
    {
        FILE* fp;
        int fd = setupFile(f, dbg, O_RDONLY | O_CLOEXEC, "r", &fp);
        r->reset(new RandomAccessFilePosix(f, fp, fd, getBlkSize(fd), file_opts));
        
    }

    IOStatus NewSequentialFilePosix(const std::string& f, 
                            const FileOptions& file_opts,
                            std::unique_ptr<FSSequentialFile>* r,
                            IODebugContext* dbg)
    {
        FILE* fp;
        int fd = setupFile(f, dbg, O_RDONLY | O_CLOEXEC, "r", &fp);
        r->reset(new SequentialFilePosix(f, fp, fd, getBlkSize(fd), file_opts));
    }

    IOStatus NewWritableFilePosix(const std::string& f, 
                            const FileOptions& file_opts,
                            std::unique_ptr<FSWritableFile>* r,
                            IODebugContext* dbg)
    {
        FILE* fp;
        int fd = setupFile(f, dbg, O_WRONLY | O_CLOEXEC, "w", &fp);
        r->reset(new WritableFilePosix(f, fp, fd, getBlkSize(fd), file_opts));
    }
    // RandomAccessFileDummy::GetRequiredBufferAlignment seems implemented
    // RandomAccessFileDummy::Prefetch perhaps we can get away with not implementing it?

    IOStatus RandomAccessFilePosix::Read(uint64_t offset, size_t n, const IOOptions& options,
				Slice* result, char* scratch,
				IODebugContext* dbg) const
    {

    }

    IOStatus SequentialFilePosix::Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
				IODebugContext* dbg) 
    {
        
    }

    IOStatus WritableFilePosix::Append(const Slice& data, const IOOptions& options,
				  IODebugContext* dbg)
    {
        
    }

    IOStatus WritableFilePosix::Close(const IOOptions& /*options*/, IODebugContext* /*dbg*/){

    }

    IOStatus WritableFilePosix::Flush(const IOOptions& options, IODebugContext* dbg){

    }
    
    IOStatus WritableFilePosix::Sync(const IOOptions& options,
                IODebugContext* dbg)
    {
        return IOStatus::OK();
    }
    
    uint64_t WritableFilePosix::GetFileSize(const IOOptions& /*options*/,
                       IODebugContext* /*dbg*/)
    {
        return 0;
    }

    IOStatus WritableFilePosix::Fsync(const IOOptions& options, IODebugContext* dbg)
    {
        return WritableFilePosix::Sync(options, dbg);
    }



    // RandomAccessFileDummy::Read 
    
    // SequentialFileDummy::Read 
    
    // WritableFileDummy::Append 
    // WritableFileDummy::Close 
    // WritableFileDummy::Flush 
    // WritableFileDummy::Fsync (implement sync instead)
    // WritableFileDummy::GetFileSize 

    // WritableFileDummy::RangeSync 
    // WritableFileDummy::SetPreallocationBlockSize 
    // WritableFileDummy::SetWriteLifeTimeHint 
    // WritableFileDummy::Sync 
    // WritableFileDummy::use_direct_io 
}

