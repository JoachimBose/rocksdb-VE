#include "posixengine.h"
#include "filedummies.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <iostream>

namespace rocksdb{
    //uncomment these to make some warnings :)

    size_t getBlkSize(int fd){
        struct stat buf;
        fstat(fd, &buf);
        return buf.st_size;
    }

    int setupFile(const std::string &f, IODebugContext* dbg, int nflags, const char* fopenMode, FILE** fp){
        int flags = O_CLOEXEC | O_CREAT | nflags;
        int fd = 0;
        std::cout << "opening \n";
        do {
            fd = open(f.c_str(), flags, 0644); 
        } while(fd < 0 && errno == EINTR);
        
        std::cout << "creating fp\n";
        if (fd > 0) {
            *fp = fdopen(fd, fopenMode);
        }
        if(fp == NULL || fd < 0)
        {
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
        int fd = setupFile(f, dbg, O_RDONLY, "r", &fp);
        r->reset(new RandomAccessFileDummy(new RandomAccessFilePosix(f, fp, fd, getBlkSize(fd), file_opts)));
        return IOStatus::OK();
    }

    IOStatus NewSequentialFilePosix(const std::string& f, 
                            const FileOptions& file_opts,
                            std::unique_ptr<FSSequentialFile>* r,
                            IODebugContext* dbg)
    {
        FILE* fp;
        int fd = setupFile(f, dbg, O_RDONLY, "r", &fp);
        r->reset(new SequentialFileDummy(new SequentialFilePosix(f, fp, fd, getBlkSize(fd), file_opts)));
        return IOStatus::OK();
    }

    IOStatus NewWritableFilePosix(const std::string& f, 
                            const FileOptions& file_opts,
                            std::unique_ptr<FSWritableFile>* r,
                            IODebugContext* dbg)
    {
        FILE* fp;
        int fd = setupFile(f, dbg, O_WRONLY, "w", &fp);
        std::cout << "creating object start\n";
        std::cout << "creating with " << fp << ", fd: " << fd << ", size: " << getBlkSize(fd);
        r->reset(new WritableFileDummy(new WritableFilePosix(f, fp, fd, getBlkSize(fd), file_opts)));
        return IOStatus::OK();
    }
    // RandomAccessFileDummy::GetRequiredBufferAlignment seems implemented
    // RandomAccessFileDummy::Prefetch perhaps we can get away with not implementing it?

    IOStatus RandomAccessFilePosix::Read(uint64_t offset, size_t n, const IOOptions& options,
				Slice* result, char* scratch,
				IODebugContext* dbg) const
    {
        std::cout << "RandomAccessFilePosix::Read\n";
    }

    IOStatus SequentialFilePosix::Read(size_t n, const IOOptions& opts, Slice* result, char* scratch,
				IODebugContext* dbg) 
    {
        std::cout << "SequentialFilePosix::Read\n";
        assert(result != nullptr && !use_direct_io());
        IOStatus s = IOStatus::OK();
        size_t r = 0;
        do {
            clearerr(this->file);
            r = fread_unlocked(scratch, 1, n, this->file);
        } while (r == 0 && ferror(this->file) && errno == EINTR);
        *result = Slice(scratch, r);
        if (r < n) {
            if (feof(this->file)) {
            // We leave status as ok if we hit the end of the file
            // We also clear the error so that the reads can continue
            // if a new data is written to the file
            clearerr(this->file);
            } else {
            // A partial read with an error: return a non-ok status
                s = IOStatus::IOError("While reading file sequentially");
            }
        }
        return s;   
    }

    inline bool IsSectorAligned(const size_t off, size_t sector_size) {
        assert((sector_size & (sector_size - 1)) == 0);
        return (off & (sector_size - 1)) == 0;
    }

    inline bool IsSectorAligned(const void* ptr, size_t sector_size) {
        return uintptr_t(ptr) % sector_size == 0;
    }

    bool PosixWrite(int fd, const char* buf, size_t nbyte) {
        const size_t kLimit1Gb = 1UL << 30;

        const char* src = buf;
        size_t left = nbyte;

        while (left != 0) {

        ssize_t done = write(fd, src, left);
        if (done < 0) {
            if (errno == EINTR) {
                continue;
            }
            std::cout << "write to " << fd <<" failed errno: " << errno << " write return: " << done << "\n";
            return false;
        }
        left -= done;
        src += done;
        }
        return true;
    }

    IOStatus WritableFilePosix::Append(const Slice& data, const IOOptions& options,
				  IODebugContext* dbg)
    {
        std::cout << "Appending\n";
        assert(IsSectorAligned(data.data(), WritableFilePosix::GetRequiredBufferAlignment()));
        assert(IsSectorAligned(data.size(), WritableFilePosix::GetRequiredBufferAlignment()));

        const char* src = data.data();
        size_t nbytes = data.size();

        if (!PosixWrite(this->fd, src, nbytes)) {
            return IOStatus::IOError("While appending to file");
        }

        this->filesize += nbytes;
        
        std::cout << "exiting appending\n";
        return IOStatus::OK();
    }

    IOStatus WritableFilePosix::Close(const IOOptions& /*options*/, IODebugContext* /*dbg*/){
        std::cout << "Close \n";

        if (close(this->fd) < 0) {
            return IOStatus::IOError("While closing file after writing");
        }
        return IOStatus::OK();
    }

    IOStatus WritableFilePosix::Flush(const IOOptions& options, IODebugContext* dbg){
        //this is implemented believe it or not
        return IOStatus::OK();
    }
    
    IOStatus WritableFilePosix::Sync(const IOOptions& options,
                IODebugContext* dbg)
    {
        if(fdatasync(this->fd) < 0){
            return IOStatus::IOError("fdatasync failed");
        }
        return IOStatus::OK();
    }
    
    uint64_t WritableFilePosix::GetFileSize(const IOOptions& /*options*/,
                       IODebugContext* /*dbg*/)
    {
        return this->filesize;
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

