/**************************************************************************//*****
 * @file     stdio.c
 * @brief    Implementation of newlib syscall
 ********************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#undef errno
extern int errno;
extern int  _end;

// Create a dummy file object to satisfy nonnull checks
static FILE dummy_stream;

/*This function is used for handle heap option*/
__attribute__ ((used))
caddr_t _sbrk(int incr) {
    static unsigned char *heap = NULL;
    unsigned char *prev_heap;
    
    // Check current stack pointer to prevent heap-stack collision
    register char * stack_ptr __asm__ ("sp");

    if (heap == NULL) {
        heap = (unsigned char *)&_end;
    }
    
    prev_heap = heap;
    
    if (heap + incr > (unsigned char *)stack_ptr) {
        // Heap and stack collision detected! Broadcast warning over USART
        char *warning = "\r\n[CRITICAL ERROR]: Heap-Stack Collision Detected!\r\n";
        while (*warning) {
            fputc(*warning++, &dummy_stream);
        }

        // Heap and stack collision!
        return (caddr_t)-1;
    }

    heap += incr;
    return (caddr_t)prev_heap;
}

__attribute__ ((used))
int link(char *old, char *new)
{
    return -1;
}

__attribute__ ((used))
int _close(int file)
{
    return -1;
}

__attribute__ ((used))
int _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

__attribute__ ((used))
int _isatty(int file)
{
    return 1;
}

__attribute__ ((used))
int _lseek(int file, int ptr, int dir)
{
    return 0;
}

/*Low layer read(input) function*/
__attribute__ ((used))
int _read(int file, char *ptr, int len)
{
    int i;
    (void)file;

    for (i = 0; i < len; i++)
    {
        // Call EmbedFire's fgetc to retrieve each character
        *ptr++ = (char)fgetc(&dummy_stream);
    }

    return len;
}

/*Low layer write(output) function*/
__attribute__ ((used))
int _write(int file, char *ptr, int len)
{
    int i;
    (void)file;

    for (i = 0; i < len; i++)
    {
        // Call fputc to output each character
        fputc(*ptr++, &dummy_stream);
    }

    return len;
}

__attribute__ ((weak))
void abort(void)
{
    /* Abort called */
    while(1);
}


int _getpid(void) {
    return 1;
}

int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    errno = EINVAL;
    return -1;
}

void _exit(int status) {
    (void)status;
    while (1) {
        // Infinite loop on exit
    }
}
/* --------------------------------- End Of File ------------------------------ */
