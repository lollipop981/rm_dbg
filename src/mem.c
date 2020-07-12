#include <sys/uio.h>
#include <stdio.h>
#include <errno.h>

#include "mem.h"
#include "utils.h"

char buffer[READ_BUFFER_SIZE] = { 0 };

int handle_memory_read_command(char *cmd, pid_t pid) {
    int ret = 0;
    long long unsigned address = 0;
    size_t length = 0;
    struct iovec local[1];
    struct iovec remote[1];

    ret = sscanf(cmd, "%llx %lu", &address, &length);
    if (ret != 2) {
        printf("Invalid input! Usage: mem {addr} {count}\n");
    }

    if (length > READ_BUFFER_SIZE) {
        printf("Max allowed number of bytes to read is %u.\n", READ_BUFFER_SIZE);
    }

    local[0].iov_base = &buffer;
    local[0].iov_len = READ_BUFFER_SIZE;
    remote[0].iov_base = (void *) address;
    remote[0].iov_len = length;
    
    ret = process_vm_readv(pid, local, 1, remote, 1, 0);
    if (ret < 0) {
        printf("Error! ");
        switch (errno)
        {
        case EFAULT:
            printf("The memory is outside the accessible address space.\n");
            /* code */
            break;
        case EINVAL:
            printf("Invalid arguments for memory read.\n");
            break;
        case ENOMEM:
            printf("Could not allocate memory for operation.\n");
            break;
        case EPERM:
            printf("The caller does not have permission to access the address space of the process with pid %u.\n", pid);
            break;
        case ESRCH:
            printf("No process with pid %u.\n", pid);
            break;
        
        default:
            printf("Unexpected error!\n");
            break;
        }

        return 1;
    }

    hex_dump(buffer, length);   
    return 0;   
}