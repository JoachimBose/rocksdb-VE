#include "aioengine.h"
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
#include "engineswap.h"

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


    IOStatus NewRandomAccessFileAio(const std::string& fname,
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

        assert(!options.use_mmap_reads);
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
                //new RandomAccessFileAio(fd, fname, base, size, options)
               
            );
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
        result->reset(new RandomAccessFileAio( fname, fd, GetLogicalBlockSizeForReadIfNeeded(options, fname, fd),
            options));
        }
        return s;
    }

    IOStatus OpenWritableFileAio(const std::string& fname,
                                    const FileOptions& options, bool reopen,
                                    std::unique_ptr<FSWritableFile>* result,
                                    IODebugContext* /*dbg*/) {
        result->reset();
        IOStatus s;
        int fd = -1;
        int flags = (reopen) ? (O_CREAT | O_APPEND) : (O_CREAT | O_TRUNC);
        flags |= O_APPEND ;

        assert(!options.use_mmap_reads);

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
        result->reset(new WritableFileAio(
            fname, fd, GetLogicalBlockSizeForWriteIfNeeded(options, fname, fd),
            options));
        } else {
        // disable mmap writes
        EnvOptions no_mmap_writes_options = options;
        no_mmap_writes_options.use_mmap_writes = false;
        result->reset(
            new WritableFileAio(fname, fd,
                                    GetLogicalBlockSizeForWriteIfNeeded(
                                        no_mmap_writes_options, fname, fd),
                                    no_mmap_writes_options));
        }
        return s;
    }

    IOStatus NewSequentialFileAio(const std::string& fname,
                             const FileOptions& options,
                             std::unique_ptr<FSSequentialFile>* result,
                             IODebugContext* /*dbg*/)
    {
        
        result->reset();
         IOStatus s;
       
        int fd = -1;
        int flags = cloexec_flags(O_RDONLY, &options);
        FILE* file = nullptr;

        assert(!options.use_mmap_reads);

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
        result->reset(new SequentialFileAio(
            fname, file, fd, GetLogicalBlockSizeForReadIfNeeded(options, fname, fd),
            options));
        return IOStatus::OK();   
    }

    IOStatus NewWritableFileAio(const std::string& fname, const FileOptions& options,
                           std::unique_ptr<FSWritableFile>* result,
                           IODebugContext* dbg)
    {
        return OpenWritableFileAio(fname, options, false, result, dbg);
    }
   

    int AioRing::AioRingRead(size_t n, Slice* result, char* scratch, int block_size, int fd, off_t read_offset)
    {
        off_t offset = 0;
        // submit requests per block
        size_t bytes_remaining = n;
        int current_block = 0;

        int blocks = (int)n / block_size;
        if (n % block_size) blocks++;
        
        //one iovec per request
        struct iovec* iovecvec = (struct iovec*)calloc(blocks, sizeof(struct iovec));
        if (!iovecvec)
        {
            std::cerr << "iovecvec calloc failed \n";
            return -1;
        }
        
        while (bytes_remaining > 0)
        {
            off_t bytes_to_read = bytes_remaining;
            if (bytes_to_read > block_size)
                bytes_to_read = block_size;
            
            iovecvec[current_block].iov_len = bytes_to_read;
            iovecvec[current_block].iov_base = (void*)(scratch + offset);
            
            offset += bytes_to_read;
            current_block++;
            bytes_remaining -= bytes_to_read;
        }
        
        struct iocb job;
        io_prep_preadv(&job, fd, iovecvec, blocks, read_offset);
        ioq[0] = &job;

        int ret = io_submit(ctx, 1, ioq);
        if (ret < 0) {
            std::cout << "io_submit err in aio write: " << -ret << "\n";
            free(iovecvec);
            return -1;
        }

        if((ret = io_getevents(ctx, 1, 1, events, &timeout)) < 0){
            std::cout << "io_getevents err whend doing aio read, code: " << -ret << "\n";
            free(iovecvec);
            return -1;
        }

        if ((ret = events[0].res) < 0) {
            std::cout << "Async readv fail result: " << ret << " target " << n << "\n";
            free(iovecvec);
            return -1;
        }
        
        *result = Slice(scratch, ret);
        free(iovecvec);
        return 0;
    } 

    int AioRing::AioRingWrite(const Slice& data, int fd, int block_size)
    {
        int current_block = 0;
        off_t offset = 0;

        int blocks = (int)data.size() / block_size;
        size_t bytes_remaining = data.size();

        if (data.size() % block_size) blocks++;
        
        struct iovec* iovecvec = (struct iovec*)calloc(blocks, sizeof(struct iovec));
        if (!iovecvec)
        {
            std::cerr << "iovecvec calloc failed \n";
            return -1;
        }
        
        size_t prev_offset = offset;
        size_t prev_bytes_to_write = bytes_remaining;
        while (bytes_remaining > 0)
        {
            off_t bytes_to_write = bytes_remaining;
            if (bytes_to_write > block_size)
                bytes_to_write = block_size;

            iovecvec[current_block].iov_len = bytes_to_write;
            iovecvec[current_block].iov_base = (void*)(data.data() + offset);

            offset += bytes_to_write;
            current_block++;
            bytes_remaining -= bytes_to_write;
        }

        struct iocb job;
        io_prep_pwritev(&job, fd, iovecvec, blocks, 0);
        ioq[0] = &job;

        int ret = io_submit(ctx, 1, ioq);
        if (ret < 0) {
            std::cout << "io_submit err in aio write" << -ret << "\n";
            free(iovecvec);
            return -1;
        }

        if((ret = io_getevents(ctx, 1, 1, events, &timeout)) < 0){
            std::cout << "io_getevent error when doing aio write, code: " << -ret << "\n";
            free(iovecvec);
            return -1;
        }

        ret = events[0].res;
        if (ret > 0 && ret != (signed long)data.size()) { // is there any situation where this should not be right?
            std::cout <<  "Async writev failed " << ret << "\n";
            std::cout << "write result: " << ret << " target " << data.size() << "\n";
            free(iovecvec);
            return -1;
        }

        free(iovecvec);
        return 0;
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

    IOStatus RandomAccessFileAio::Read(uint64_t offset, size_t n,
                                     const IOOptions& /*opts*/, Slice* result,
                                     char* scratch,
                                     IODebugContext* /*dbg*/) const {
        DTRACE_PROBE(libaio, rdread);
        if (use_direct_io()) {
            assert(IsSectorAligned(offset, GetRequiredBufferAlignment()));
            assert(IsSectorAligned(n, GetRequiredBufferAlignment()));
            assert(IsSectorAligned(scratch, GetRequiredBufferAlignment()));
        }
        IOStatus s;
        ssize_t r = -1;
        
        std::unique_ptr<AioRing>* ring = EngineSwapFileSystem::getAioRing();
        r = ring->get()->AioRingRead(n, result, scratch, logical_sector_size_, fd_, offset);
        if (r < 0) {
            // An error: return a non-ok status
            return IOError("While random pread offset " + std::to_string(offset) + " len " +
                            std::to_string(n),
                        filename_, errno);
        }
        return s;
    }
    IOStatus SequentialFileAio::Read(size_t n, const IOOptions& /*opts*/,
                                   Slice* result, char* scratch,
                                   IODebugContext* /*dbg*/) {
        DTRACE_PROBE(libaio, sqread);
        assert(result != nullptr && !use_direct_io());
        IOStatus s = IOStatus::OK();
        int r = 0;
        // do {
        //     clearerr(file_);
        //     r = fread_unlocked(scratch, 1, n, file_);
        // } while (r == 0 && ferror(file_) && errno == EINTR);
        std::unique_ptr<AioRing>* ring = EngineSwapFileSystem::getAioRing();
        r = ring->get()->AioRingRead(n, result, scratch, logical_sector_size_, fd_, currentByte);
        if (r < 0) {
            return IOError("While sequentially reading from", PosixSequentialFile::filename_, errno);
        }
        currentByte += result->size();
        return s;
        }

    IOStatus WritableFileAio::Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) {
        DTRACE_PROBE(libaio, write);
       // std::cerr << "Append called\n";
        if (use_direct_io()) {
            assert(IsSectorAligned(data.size(), GetRequiredBufferAlignment()));
            assert(IsSectorAligned(data.data(), GetRequiredBufferAlignment()));
        }
        uint64_t nbytes = data.size();
        std::unique_ptr<AioRing>* ring = EngineSwapFileSystem::getAioRing();
        // std::cout << "ring: " << ring->get() << " sector size: " << logical_sector_size_ << "\n";

        if (ring->get()->AioRingWrite(data, fd_, logical_sector_size_) != 0) {
            return IOError("While appending to file", PosixWritableFile::filename_, errno);
        }

        PosixWritableFile::filesize_ += nbytes;
        
        return IOStatus::OK();
    }
}

