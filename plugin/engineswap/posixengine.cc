#include "posixengine.h"
#include "filedummies.h"
#include "util.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <sys/sdt.h>
#include <iostream>
#include <sys/statfs.h>

#if !defined(TMPFS_MAGIC)
#define TMPFS_MAGIC 0x01021994
#endif
#if !defined(XFS_SUPER_MAGIC)
#define XFS_SUPER_MAGIC 0x58465342
#endif
#if !defined(EXT4_SUPER_MAGIC)
#define EXT4_SUPER_MAGIC 0xEF53
#endif


namespace rocksdb{
    //uncomment these to make some warnings :)

    IOStatus NewRandomAccessFilePosix(const std::string& fname,
                               const FileOptions& options,
                               std::unique_ptr<FSRandomAccessFile>* result,
                               IODebugContext* /*dbg*/)
    {
        IOStatus s = IOStatus::OK();
        int fd;
        int flags = cloexec_flags(O_RDONLY, &options);

        if (options.use_direct_reads && !options.use_mmap_reads) {
    #if !defined(OS_MACOSX) && !defined(OS_OPENBSD) && !defined(OS_SOLARIS)
        flags |= O_DIRECT;
        TEST_SYNC_POINT_CALLBACK("NewRandomAccessFile:O_DIRECT", &flags);
    #endif
        }

        do {
        // IOSTATS_TIMER_GUARD(open_nanos);
        fd = open(fname.c_str(), flags, 0644);
        } while (fd < 0 && errno == EINTR);
        if (fd < 0) {
        s = IOError("While open a file for random read", fname, errno);
        return s;
        }
        SetFD_CLOEXEC(fd, &options);

        if (options.use_mmap_reads) {
        // Use of mmap for random reads has been removed because it
        // kills performance when storage is fast.
        // Use mmap when virtual address-space is plentiful.
        uint64_t size;
        IOOptions opts;
        s = GetFileSize(fname, opts, &size, nullptr);
        if (s.ok()) {
            std::cout << "its going to make an mmapped file, but we dont support\n";
            char* p = nullptr;
            std::cout << *p; //die
            void* base = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
            if (base != MAP_FAILED) {
            result->reset(
                new PosixMmapReadableFile(fd, fname, base, size, options));
            } else {
            s = IOError("while mmap file for read", fname, errno);
            close(fd);
            }
        } else {
            close(fd);
        }
        } else {
        if (options.use_direct_reads && !options.use_mmap_reads) {

        }
        result->reset(new RandomAccessFilePosix(
            fname, fd, GetLogicalBlockSizeForReadIfNeeded(options, fname, fd),
            options));
        }
        return s;
    }

    IOStatus OpenWritableFilePosix(const std::string& fname,
                                    const FileOptions& options, bool reopen,
                                    std::unique_ptr<FSWritableFile>* result,
                                    IODebugContext* /*dbg*/) {
        result->reset();
        IOStatus s;
        int fd = -1;
        int flags = (reopen) ? (O_CREAT | O_APPEND) : (O_CREAT | O_TRUNC);
        // Direct IO mode with O_DIRECT flag or F_NOCAHCE (MAC OSX)
        if (options.use_direct_writes && !options.use_mmap_writes) {
        // Note: we should avoid O_APPEND here due to ta the following bug:
        // POSIX requires that opening a file with the O_APPEND flag should
        // have no affect on the location at which pwrite() writes data.
        // However, on Linux, if a file is opened with O_APPEND, pwrite()
        // appends data to the end of the file, regardless of the value of
        // offset.
        // More info here: https://linux.die.net/man/2/pwrite
        flags |= O_WRONLY;
    #if !defined(OS_MACOSX) && !defined(OS_OPENBSD) && !defined(OS_SOLARIS)
        flags |= O_DIRECT;
    #endif
        TEST_SYNC_POINT_CALLBACK("NewWritableFile:O_DIRECT", &flags);
        } else if (options.use_mmap_writes) {
        // non-direct I/O
        flags |= O_RDWR;
        } else {
        flags |= O_WRONLY;
        }

        flags = cloexec_flags(flags, &options);

        do {
        // IOSTATS_TIMER_GUARD(open_nanos);
        fd = open(fname.c_str(), flags, 0644);
        } while (fd < 0 && errno == EINTR);

        if (fd < 0) {
        s = IOError("While open a file for appending", fname, errno);
        return s;
        }
        SetFD_CLOEXEC(fd, &options);

        if (options.use_mmap_writes) {
        MaybeForceDisableMmap(fd);
        }
        if (options.use_mmap_writes && !false) {
        result->reset(new PosixMmapFile(fname, fd, 4096, options));
        } else if (options.use_direct_writes && !options.use_mmap_writes) {
    #ifdef OS_MACOSX
        if (fcntl(fd, F_NOCACHE, 1) == -1) {
            close(fd);
            s = IOError("While fcntl NoCache an opened file for appending", fname,
                        errno);
            return s;
        }
    #elif defined(OS_SOLARIS)
        if (directio(fd, DIRECTIO_ON) == -1) {
            if (errno != ENOTTY) {  // ZFS filesystems don't support DIRECTIO_ON
            close(fd);
            s = IOError("While calling directio()", fname, errno);
            return s;
            }
        }
    #endif
        result->reset(new WritableFilePosix(
            fname, fd, GetLogicalBlockSizeForWriteIfNeeded(options, fname, fd),
            options));
        } else {
        // disable mmap writes
        EnvOptions no_mmap_writes_options = options;
        no_mmap_writes_options.use_mmap_writes = false;
        result->reset(
            new WritableFilePosix(fname, fd,
                                    GetLogicalBlockSizeForWriteIfNeeded(
                                        no_mmap_writes_options, fname, fd),
                                    no_mmap_writes_options));
        }
        return s;
    }

    IOStatus NewSequentialFilePosix(const std::string& fname,
                             const FileOptions& options,
                             std::unique_ptr<FSSequentialFile>* result,
                             IODebugContext* /*dbg*/)
    {
        
        result->reset();
        int fd = -1;
        int flags = cloexec_flags(O_RDONLY, &options);
        FILE* file = nullptr;

        if (options.use_direct_reads && !options.use_mmap_reads) {
    #if !defined(OS_MACOSX) && !defined(OS_OPENBSD) && !defined(OS_SOLARIS)
        flags |= O_DIRECT;
        TEST_SYNC_POINT_CALLBACK("NewSequentialFile:O_DIRECT", &flags);
    #endif
        }

        do {
        // IOSTATS_TIMER_GUARD(open_nanos);
        fd = open(fname.c_str(), flags, 0644);
        } while (fd < 0 && errno == EINTR);
        if (fd < 0) {
        return IOError("While opening a file for sequentially reading", fname,
                        errno);
        }

        SetFD_CLOEXEC(fd, &options);

        if (options.use_direct_reads && !options.use_mmap_reads) {
    
        } else {
        do {
            // IOSTATS_TIMER_GUARD(open_nanos);
            file = fdopen(fd, "r");
        } while (file == nullptr && errno == EINTR);
        if (file == nullptr) {
            close(fd);
            return IOError("While opening file for sequentially read", fname,
                        errno);
        }
        }
        result->reset(new PosixSequentialFile(
            fname, file, fd, GetLogicalBlockSizeForReadIfNeeded(options, fname, fd),
            options));
        return IOStatus::OK();   
    }

    IOStatus NewWritableFilePosix(const std::string& fname, const FileOptions& options,
                           std::unique_ptr<FSWritableFile>* result,
                           IODebugContext* dbg)
    {
        return OpenWritableFilePosix(fname, options, true, result, dbg);
    }

    IOStatus RandomAccessFilePosix::Read(uint64_t offset, size_t n,
                                     const IOOptions& /*opts*/, Slice* result,
                                     char* scratch,
                                     IODebugContext* /*dbg*/) const {
        DTRACE_PROBE(posix, rdread);   
        if (use_direct_io()) {
            assert(IsSectorAligned(offset, GetRequiredBufferAlignment()));
            assert(IsSectorAligned(n, GetRequiredBufferAlignment()));
            assert(IsSectorAligned(scratch, GetRequiredBufferAlignment()));
        }
        IOStatus s;
        ssize_t r = -1;
        size_t left = n;
        char* ptr = scratch;
        while (left > 0) {
            r = pread(fd_, ptr, left, static_cast<off_t>(offset));
            if (r <= 0) {
            if (r == -1 && errno == EINTR) {
                continue;
            }
            break;
            }
            ptr += r;
            offset += r;
            left -= r;
            if (use_direct_io() &&
                r % static_cast<ssize_t>(GetRequiredBufferAlignment()) != 0) {
            // Bytes reads don't fill sectors. Should only happen at the end
            // of the file.
            break;
            }
        }
        if (r < 0) {
            // An error: return a non-ok status
            s = IOError("While pread offset " + std::to_string(offset) + " len " +
                            std::to_string(n),
                        filename_, errno);
        }
        *result = Slice(scratch, (r < 0) ? 0 : n - left);
        return s;                             
        
    }
    IOStatus SequentialFilePosix::Read(size_t n, const IOOptions& /*opts*/,
                                   Slice* result, char* scratch,
                                   IODebugContext* /*dbg*/) {
        DTRACE_PROBE(posix, sqread);
        assert(result != nullptr && !use_direct_io());
        IOStatus s;
        size_t r = 0;
        do {
            clearerr(file_);
            r = fread_unlocked(scratch, 1, n, file_);
        } while (r == 0 && ferror(file_) && errno == EINTR);
        *result = Slice(scratch, r);
        if (r < n) {
            if (feof(file_)) {
            // We leave status as ok if we hit the end of the file
            // We also clear the error so that the reads can continue
            // if a new data is written to the file
            clearerr(file_);
            } else {
            // A partial read with an error: return a non-ok status
            s = IOError("While reading file sequentially", filename_, errno);
            }
        }
        return s;   
    }

    bool PosixWriteCustom(int fd, const char* buf, size_t nbyte) {
        const size_t kLimit1Gb = 1UL << 30;

        const char* src = buf;
        size_t left = nbyte;

        while (left != 0) {
            size_t bytes_to_write = std::min(left, kLimit1Gb);

            ssize_t done = write(fd, src, bytes_to_write);
            if (done < 0) {
            if (errno == EINTR) {
                continue;
            }
            return false;
            }
            left -= done;
            src += done;
        }
        return true;
    }
    
    IOStatus WritableFilePosix::Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) {
        
        DTRACE_PROBE(posix, write);

        if (use_direct_io()) {
            assert(IsSectorAligned(data.size(), GetRequiredBufferAlignment()));
            assert(IsSectorAligned(data.data(), GetRequiredBufferAlignment()));
        }
        const char* src = data.data();
        size_t nbytes = data.size();

        if (!PosixWriteCustom(fd_, src, nbytes)) {
            return IOError("While appending to file", filename_, errno);
        }

        filesize_ += nbytes;
        return IOStatus::OK();
    }
    // RandomAccessFileDummy::GetRequiredBufferAlignment seems implemented
    // RandomAccessFileDummy::Prefetch perhaps we can get away with not implementing it?


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

