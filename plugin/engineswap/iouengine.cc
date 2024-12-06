#include "iouengine.h"
#include "filedummies.h"
#include "filealarms.h"
#include "util.h"
#include "engineswap.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>
#include <iostream>
#include <sys/statfs.h>
#include <sys/sdt.h>

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

    
    IOStatus NewRandomAccessFileIou(const std::string& fname,
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
        result->reset(new RandomAccessFileAlarm(new RandomAccessFileIou(
            fname, fd, GetLogicalBlockSizeForReadIfNeeded(options, fname, fd),
            options)));
        }
        return s;
    }

    IOStatus OpenWritableFileIou(const std::string& fname,
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
        result->reset(new WritableFileAlarm(new WritableFileIou(
            fname, fd, GetLogicalBlockSizeForWriteIfNeeded(options, fname, fd),
            options)));
        } else {
            // disable mmap writes
            EnvOptions no_mmap_writes_options = options;
            no_mmap_writes_options.use_mmap_writes = false;
            result->reset(
                new WritableFileAlarm (new WritableFileIou(fname, fd,
                                        GetLogicalBlockSizeForWriteIfNeeded(
                                            no_mmap_writes_options, fname, fd),
                                        no_mmap_writes_options)));
        }
        return s;
    }

    IOStatus NewSequentialFileIou(const std::string& fname,
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
        result->reset(new SequentialFileAlarm(new SequentialFileIou(
            fname, file, fd, GetLogicalBlockSizeForReadIfNeeded(options, fname, fd),
            options)));
        return IOStatus::OK();   
    }

    IOStatus NewWritableFileIou(const std::string& fname, const FileOptions& options,
                           std::unique_ptr<FSWritableFile>* result,
                           IODebugContext* dbg)
    {
        return OpenWritableFileIou(fname, options, true, result, dbg); //modified reopen
    }

    int IouRing::IouRingRead(size_t n, Slice* result, char* scratch, int block_size, int fd, off_t read_offset)
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
        
        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_readv(sqe, fd, iovecvec, blocks, read_offset);
        io_uring_sqe_set_data(sqe, iovecvec);
        io_uring_submit(&this->ring);

        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            std::cout << ("io_uring_wait_cqe\n");
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return -1;
        }
        if (cqe->res < 0) {
            std::cout << "Async readv fail result: " << cqe->res << " target " << n << "\n";
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return -1;
        }

        iovecvec = (struct iovec*)io_uring_cqe_get_data(cqe);
        
        *result = Slice(scratch, cqe->res);
        free(iovecvec);
        io_uring_cqe_seen(&ring, cqe); //stuff in the cqe cannot be accessed anymore, 
        return 0;
    } 

    int IouRing::IouRingWrite(const Slice& data, int fd, int block_size)
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

        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        io_uring_prep_writev(sqe, fd, iovecvec, blocks, 0);
        io_uring_sqe_set_data(sqe, iovecvec);
        io_uring_submit(&this->ring);

        struct io_uring_cqe *cqe;
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) {
            std::cout <<"io_uring_wait_cqe \n";
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return -1;
        }
        if (cqe->res > 0 && cqe->res != (signed long)data.size()) { // is there any situation where this should not be right?
            std::cout <<  "Async writev failed " << cqe->res << "\n";
            std::cout << "write result: " << cqe->res << " target " << data.size() << "\n";
            io_uring_cqe_seen(&ring, cqe);
            free(iovecvec);
            return -1;
        }

        io_uring_cqe_seen(&ring, cqe);
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

    IOStatus RandomAccessFileIou::Read(uint64_t offset, size_t n,
                                     const IOOptions& /*opts*/, Slice* result,
                                     char* scratch,
                                     IODebugContext* /*dbg*/) const {
        if (use_direct_io()) {
            assert(IsSectorAligned(offset, GetRequiredBufferAlignment()));
            assert(IsSectorAligned(n, GetRequiredBufferAlignment()));
            assert(IsSectorAligned(scratch, GetRequiredBufferAlignment()));
        }
        IOStatus s;
        ssize_t r = -1;
        
        std::unique_ptr<IouRing>* ring = EngineSwapFileSystem::getRing();
        r = ring->get()->IouRingRead(n, result, scratch, logical_sector_size_, fd_, offset);
        if (r < 0) {
            // An error: return a non-ok status
            return IOError("While random pread offset " + std::to_string(offset) + " len " +
                            std::to_string(n),
                        filename_, errno);
        }
        return s;
    }
    IOStatus SequentialFileIou::Read(size_t n, const IOOptions& /*opts*/,
                                   Slice* result, char* scratch,
                                   IODebugContext* /*dbg*/) {
        
        assert(result != nullptr && !use_direct_io());
        IOStatus s = IOStatus::OK();
        int r = 0;
        // do {
        //     clearerr(file_);
        //     r = fread_unlocked(scratch, 1, n, file_);
        // } while (r == 0 && ferror(file_) && errno == EINTR);
        std::unique_ptr<IouRing>* ring = EngineSwapFileSystem::getRing();
        r = ring->get()->IouRingRead(n, result, scratch, logical_sector_size_, fd_, currentByte);
        if (r < 0) {
            return IOError("While sequentially reading from", PosixSequentialFile::filename_, errno);
        }
        currentByte += result->size();
        return s;
        }

    uint64_t getActualFileSize(int fd){
        struct stat64 s;
        fstat64(fd, &s);
        return s.st_size;
    }
    
    IOStatus WritableFileIou::Append(const Slice& data, const IOOptions& /*opts*/,
                                   IODebugContext* /*dbg*/) {
        
        DTRACE_PROBE(iouengine, write);

        if (use_direct_io()) {
            assert(IsSectorAligned(data.size(), GetRequiredBufferAlignment()));
            assert(IsSectorAligned(data.data(), GetRequiredBufferAlignment()));
        }
        uint64_t nbytes = data.size();
        std::unique_ptr<IouRing>* ring = EngineSwapFileSystem::getRing();
        // std::cout << "ring: " << ring->get() << " sector size: " << logical_sector_size_ << "\n";

        if (ring->get()->IouRingWrite(data, fd_, logical_sector_size_) != 0) {
            return IOError("While appending to file", PosixWritableFile::filename_, errno);
        }

        PosixWritableFile::filesize_ += nbytes;
        
        return IOStatus::OK();
    }
}

