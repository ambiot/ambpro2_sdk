#include "platform_conf.h"
#undef ROM_REGION
#include <stdarg.h>
#include "cmsis_compiler.h"
#include "stdio_port_func.h"

#if defined(__GNUC__)

// Set picolibc __iob api for semihosting and emit the alias name to _times.
#ifdef _PICOLIBC__
//#include <time.h>
#include <stdio.h>

//clock_t times (struct tms * tp) __attribute__((alias("_times")));

static int dev_putc(char c, FILE *file)
{
	(void) file;
	stdio_port_putc(c);
	return 0;
}


static FILE __stdio = FDEV_SETUP_STREAM(dev_putc, NULL, NULL, _FDEV_SETUP_RW);
FILE *const __iob[3] = { &__stdio, &__stdio, &__stdio };
#endif

#endif


#if defined ( __ICCARM__ )
#include <LowLevelIOInterface.h>
// Lowlevel IO redirect wrapper for IAR
#include <stdio.h>
#include "ff.h"


static int handle = 3;

#define MAX_FILES 8 	// 0, 1, 2 is NULL
static FIL *__fatfs_fil[MAX_FILES] = {0};

int __open(const char *filename, int mode)
{
	uint8_t mode_mapping = 0;
	uint8_t do_seekend = 0;

	if (mode & _LLIO_CREAT) {
		/* Create a file if it doesn't exists. */
		mode_mapping |= FA_CREATE_ALWAYS;

		/* Check what we should do with it if it exists. */
		if (mode & _LLIO_APPEND) {
			/* Append to the existing file. */
			do_seekend = 1;
		}

		if (mode & _LLIO_TRUNC) {
			/* Truncate the existsing file. */
			mode_mapping |= FA_OPEN_ALWAYS;
		}
	}

	if (mode & _LLIO_TEXT) {
		/* The file should be opened in text form. */
	} else {
		/* The file should be opened in binary form. */
	}

	switch (mode & _LLIO_RDWRMASK) {
	case _LLIO_RDONLY:
		/* The file should be opened for read only. */
		mode_mapping |= FA_READ;
		break;

	case _LLIO_WRONLY:
		/* The file should be opened for write only. */
		mode_mapping |= FA_WRITE;
		break;

	case _LLIO_RDWR:
		/* The file should be opened for both reads and writes. */
		mode_mapping |= (FA_READ | FA_WRITE);
		break;

	default:
		return -1;
	}

	/*
	* Add the code for opening the file here.
	*/
	int free_handle = -1;
	for (int i = handle; i < MAX_FILES; i++) {
		if (__fatfs_fil[i] == NULL) {
			free_handle = i;
			break;
		}
	}

	if (free_handle == -1)	{
		return -1;
	}

	__fatfs_fil[free_handle] = malloc(sizeof(FIL));
	if (__fatfs_fil[free_handle] == NULL)	{
		return -1;
	}

	FRESULT res = f_open(__fatfs_fil[free_handle], filename, mode_mapping);
	if (res != 0) {
		free(__fatfs_fil[free_handle]);
		__fatfs_fil[free_handle] = NULL;
		return -1;
	}
	if (do_seekend == 1) {
		f_lseek(__fatfs_fil[free_handle], f_size(__fatfs_fil[free_handle])); // Move r/w pointer to end of the file
	}
	//printf("Open file mode 0x%x\n\r", mode_mapping);

	return free_handle;
}

int __close(int handle)
{
	FRESULT res = f_close(__fatfs_fil[handle]);
	free(__fatfs_fil[handle]);
	__fatfs_fil[handle] = NULL;

	if (res == 0) {
		return 0;
	} else {
		return -res;
	}
}

size_t __read(int handle, unsigned char *buffer, size_t size)
{

	if (handle != _LLIO_STDIN) {
		// FATFS
		size_t br;
		FRESULT res = f_read(__fatfs_fil[handle], buffer, size, &br);
		if (res > 0) {
			return -1;
		}

		return br;
	} else {
		// STDIN
		int nChars = 0;
		for (/* Empty */; size > 0; --size) {
			int c = stdio_port_getc(_FALSE);
			if (c < 0) {
				break;
			}

			*buffer++ = c;
			++nChars;
		}
		return nChars;
	}
}


size_t __write(int handle, const unsigned char *buffer, size_t size)
{

	size_t nChars = 0;

	if (buffer == 0) {
		/*
		* This means that we should flush internal buffers.  Since we
		* don't we just return.  (Remember, "handle" == -1 means that all
		* handles should be flushed.)
		*/
		if (handle >= 3) {
			int res = f_sync(__fatfs_fil[handle]);
			return -res;
		}

		if (handle == -1) {
			int res = 0;
			for (int i = 3; i < MAX_FILES; i++) {
				res += f_sync(__fatfs_fil[i]);
			}
			return -res;
		}

		return 0;
	}

	/* This template only writes to "standard out" and "standard err",
	* for all other file handles it returns failure. */
	if (handle != _LLIO_STDOUT && handle != _LLIO_STDERR) {
		// only support size = 1byte
		size_t bw;

		FRESULT res = f_write(__fatfs_fil[handle], buffer, size, &bw);
		if (res > 0) {
			return -1;
		}

		return bw;
	}

	for (/* Empty */; size != 0; --size) {
		if (*buffer == '\n') {
			stdio_port_putc('\r');
		}
		stdio_port_putc(*buffer++);
		++nChars;
	}

	return nChars;
}


long __lseek(int handle, long offset, int whence)
{

	if (handle <= _LLIO_STDERR)	{
		return 0;
	}

	int size = f_size(__fatfs_fil[handle]);
	int curr = f_tell(__fatfs_fil[handle]);
	int res = -1;

	switch (whence) {
	case SEEK_SET:
		res = f_lseek(__fatfs_fil[handle], offset);
		break;
	case SEEK_CUR:
		res = f_lseek(__fatfs_fil[handle], curr + offset);
		break;
	case SEEK_END:
		res = f_lseek(__fatfs_fil[handle], size - offset);
		break;
	}

	if (res < 0)	{
		return 0;
	}
	return f_tell(__fatfs_fil[handle]);
}

int remove(const char *filename)
{
	FRESULT res = f_unlink(filename);
	return -res;
}

int rename(const char *old, const char *new)
{
	FRESULT res = f_rename(old, new);
	return -res;
}

#if 0
size_t __write(int Handle, const unsigned char *Buf, size_t Bufsize)
{
	int nChars = 0;
	/* Check for stdout and stderr
	(only necessary if file descriptors are enabled.) */
	if (Handle != 1 && Handle != 2) {
		return -1;
	}
	for (/*Empty */; Bufsize > 0; --Bufsize) {
		stdio_port_putc(*Buf++);
		++nChars;
	}
	return nChars;
}

size_t __read(int Handle, unsigned char *Buf, size_t Bufsize)
{
	int nChars = 0;
	/* Check for stdin
	(only necessary if FILE descriptors are enabled) */
	if (Handle != 0) {
		return -1;
	}
	for (/*Empty*/; Bufsize > 0; --Bufsize) {
		int c = stdio_port_getc(_FALSE);
		if (c < 0) {
			break;
		}
		*(Buf++) = c;
		++nChars;
	}
	return nChars;
}
#endif
#endif