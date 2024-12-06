#include "util.h"
#include "env/io_posix.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/sysmacros.h>

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
    bool forceMmapOff_ = false;
    size_t GetLogicalBlockSizeOfFd(int fd) {
        #ifdef OS_LINUX
        struct stat buf;
        int result = fstat(fd, &buf);
        if (result == -1) {
            return kDefaultPageSize;
        }
        if (major(buf.st_dev) == 0) {
            // Unnamed devices (e.g. non-device mounts), reserved as null device number.
            // These don't have an entry in /sys/dev/block/. Return a sensible default.
            return kDefaultPageSize;
        }

        // Reading queue/logical_block_size does not require special permissions.
        const int kBufferSize = 100;
        char path[kBufferSize];
        char real_path[PATH_MAX + 1];
        snprintf(path, kBufferSize, "/sys/dev/block/%u:%u", major(buf.st_dev),
                minor(buf.st_dev));
        if (realpath(path, real_path) == nullptr) {
            return kDefaultPageSize;
        }
        std::string device_dir(real_path);
        if (!device_dir.empty() && device_dir.back() == '/') {
            device_dir.pop_back();
        }
        // NOTE: sda3 and nvme0n1p1 do not have a `queue/` subdir, only the parent sda
        // and nvme0n1 have it.
        // $ ls -al '/sys/dev/block/8:3'
        // lrwxrwxrwx. 1 root root 0 Jun 26 01:38 /sys/dev/block/8:3 ->
        // ../../block/sda/sda3
        // $ ls -al '/sys/dev/block/259:4'
        // lrwxrwxrwx 1 root root 0 Jan 31 16:04 /sys/dev/block/259:4 ->
        // ../../devices/pci0000:17/0000:17:00.0/0000:18:00.0/nvme/nvme0/nvme0n1/nvme0n1p1
        size_t parent_end = device_dir.rfind('/', device_dir.length() - 1);
        if (parent_end == std::string::npos) {
            return kDefaultPageSize;
        }
        size_t parent_begin = device_dir.rfind('/', parent_end - 1);
        if (parent_begin == std::string::npos) {
            return kDefaultPageSize;
        }
        std::string parent =
            device_dir.substr(parent_begin + 1, parent_end - parent_begin - 1);
        std::string child = device_dir.substr(parent_end + 1, std::string::npos);
        if (parent != "block" &&
            (child.compare(0, 4, "nvme") || child.find('p') != std::string::npos)) {
            device_dir = device_dir.substr(0, parent_end);
        }
        std::string fname = device_dir + "/queue/logical_block_size";
        FILE* fp;
        size_t size = 0;
        fp = fopen(fname.c_str(), "r");
        if (fp != nullptr) {
            char* line = nullptr;
            size_t len = 0;
            if (getline(&line, &len, fp) != -1) {
            sscanf(line, "%zu", &size);
            }
            free(line);
            fclose(fp);
        }
        if (size != 0 && (size & (size - 1)) == 0) {
            return size;
        }
        #endif
        (void)fd;
        return kDefaultPageSize;
    }


    #ifdef OS_LINUX
    static LogicalBlockSizeCache logical_block_size_cache_;
    #endif

    size_t GetLogicalBlockSize(const std::string& fname, int fd) {
        #ifdef OS_LINUX
        return logical_block_size_cache_.GetLogicalBlockSize(fname, fd);
        #else
        (void)fname;
        return GetLogicalBlockSizeOfFd(fd);
        #endif
    }

    size_t GetLogicalBlockSizeForReadIfNeeded(
        const EnvOptions& options, const std::string& fname, int fd) {
    return options.use_direct_reads
                ? GetLogicalBlockSize(fname, fd)
                : kDefaultPageSize;
    }

    size_t GetLogicalBlockSizeForWriteIfNeeded(
        const EnvOptions& options, const std::string& fname, int fd) {
    return options.use_direct_writes
                ? GetLogicalBlockSize(fname, fd)
                : kDefaultPageSize;
    }

    int cloexec_flags(int flags, const EnvOptions* options) {
    // If the system supports opening the file with cloexec enabled,
    // do so, as this avoids a race condition if a db is opened around
    // the same time that a child process is forked
        #ifdef O_CLOEXEC
        if (options == nullptr || options->set_fd_cloexec) {
            flags |= O_CLOEXEC;
        }
        #else
        (void)options;
        #endif
        return flags;
    }

    bool SupportsFastAllocate(int fd) {
    #ifdef ROCKSDB_FALLOCATE_PRESENT
        struct statfs s;
        if (fstatfs(fd, &s)) {
        return false;
        }
        switch (s.f_type) {
        case EXT4_SUPER_MAGIC:
            return true;
        case XFS_SUPER_MAGIC:
            return true;
        case TMPFS_MAGIC:
            return true;
        default:
            return false;
        }
    #else
        (void)fd;
        return false;
    #endif
    }

    void MaybeForceDisableMmap(int fd) {
        static std::once_flag s_check_disk_for_mmap_once;
        // assert(this == FileSystem::Default().get());
        if (!SupportsFastAllocate(fd)) 
        {
            forceMmapOff_ = true;
        }       
    }

    void SetFD_CLOEXEC(int fd, const EnvOptions* options) {
        if ((options == nullptr || options->set_fd_cloexec) && fd > 0) {
        fcntl(fd, F_SETFD, fcntl(fd, F_GETFD) | FD_CLOEXEC);
        }
    }

    IOStatus GetFileSize(const std::string& fname, const IOOptions& /*opts*/,
                       uint64_t* size, IODebugContext* /*dbg*/) {
        struct stat sbuf;
        if (stat(fname.c_str(), &sbuf) != 0) {
        *size = 0;
        return IOError("while stat a file for size", fname, errno);
        } else {
        *size = sbuf.st_size;
        }
        return IOStatus::OK();
    }
}