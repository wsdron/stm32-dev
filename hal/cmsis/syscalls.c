#include <sys/stat.h>
#include <errno.h>

#undef errno
extern int errno;

int _close(int file) {
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _read(int file, char *ptr, int len) {
    return 0;
}

int _write(int file, char *ptr, int len) {
    return len;
}

caddr_t _sbrk(int incr) {
    extern char _end;
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;
    if ((heap_end + incr) > (char*)0x20005000) { //  __StackLimit = 0x20005000 in your linker script
        errno = ENOMEM;
        return (caddr_t) -1;
    }
    heap_end += incr;
    return (caddr_t) prev_heap_end;
}
