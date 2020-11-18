#include "semihosting.h"

long semihosting_call(unsigned long operation,
        void *system_block_address) {
    register long value asm( "r0" ) = operation;
    register void *ptr asm( "r1" ) = system_block_address;

    asm volatile( "svc #0x123456" : "+r"(value): "r"(ptr): "memory");
    return value;
}

typedef struct {
    const char *file_name;
    unsigned long mode;
    size_t name_length;
} smh_file_open_block_t;

typedef struct {
    long handle;
    uintptr_t buffer;
    size_t length;
} smh_file_read_write_block_t;

typedef struct {
    long handle;
    ssize_t location;
} smh_file_seek_block_t;

typedef struct {
    char *command_line;
    size_t command_length;
} smh_system_block_t;

long semihosting_file_open(const char *file_name, size_t mode)
{
    smh_file_open_block_t open_block;

    open_block.file_name = file_name;
    open_block.mode = mode;
    open_block.name_length = strlen(file_name);

    return semihosting_call(SEMIHOSTING_SYS_OPEN,
            (void *) &open_block);
}

long semihosting_file_seek(long file_handle, ssize_t offset)
{
    smh_file_seek_block_t seek_block;
    long result;

    seek_block.handle = file_handle;
    seek_block.location = offset;

    result = semihosting_call(SEMIHOSTING_SYS_SEEK,
            (void *) &seek_block);

    if (result)
        result = semihosting_call(SEMIHOSTING_SYS_ERRNO, 0);

    return result;
}

long semihosting_file_read(long file_handle, uintptr_t buffer, size_t *length)
{
    smh_file_read_write_block_t read_block;
    long result = -EINVAL;

    if ((length == NULL) || (buffer == (uintptr_t)NULL))
        return result;

    read_block.handle = file_handle;
    read_block.buffer = buffer;
    read_block.length = *length;

    result = semihosting_call(SEMIHOSTING_SYS_READ,
            (void *) &read_block);

    if (result == *length) {
        return -EINVAL;
    } else if (result < *length) {
        *length -= result;
        return 0;
    } else
        return result;
}

long semihosting_file_write(long file_handle,
        const uintptr_t buffer,
        size_t *length)
{
    smh_file_read_write_block_t write_block;

    if ((length == NULL) || (buffer == (uintptr_t)NULL))
        return -EINVAL;

    write_block.handle = file_handle;
    write_block.buffer = (uintptr_t)buffer; /* cast away const */
    write_block.length = *length;

    *length = semihosting_call(SEMIHOSTING_SYS_WRITE,
            (void *) &write_block);

    return *length;
}

long semihosting_file_close(long file_handle)
{
    return semihosting_call(SEMIHOSTING_SYS_CLOSE,
            (void *) &file_handle);
}

long semihosting_file_length(long file_handle)
{
    return semihosting_call(SEMIHOSTING_SYS_FLEN,
            (void *) &file_handle);
}
