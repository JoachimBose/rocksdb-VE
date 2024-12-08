#include <liburing.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "rocksdb/slice.h"

#define QUEUE_ITEMS 4

namespace rocksdb
{
    class IouRing{
    struct io_uring ring;
    public:
    IouRing(){
        struct io_uring_params params;

        memset(&params, 0, sizeof(params));
        params.flags |= IORING_SETUP_SQPOLL;
        // params.flags |= IORING_SETUP_IOPOLL;
        params.sq_thread_idle = 5000;

        int ret = io_uring_queue_init_params(QUEUE_ITEMS, &ring, &params);

        if(ret != 0){
            fprintf(stderr, "que init failed %i \n errno = %i\n errno: %s\n", ret, errno, strerror(errno));
        }
        else{
        // std::cout << "init success \n
        }
    }
    ~IouRing(){
        io_uring_queue_exit(&ring);
    }
    int IouRingWrite(const Slice& data, int fd, int block_size);
    int IouRingRead(size_t n, Slice* result, char* scratch, int block_size, int fd, off_t offset);
    int IouNvRingWrite(const Slice& data, int fd);
    int IouNvRingRead(size_t n, Slice* result, char* scratch, int fd, off_t offset);
    };
}