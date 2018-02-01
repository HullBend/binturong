

#ifndef __DISK_UTIL_H__
#define __DISK_UTIL_H__


#include "stdafx.h"



#ifdef __cplusplus
extern "C" {
#endif


/**
 * disk_mkPath - Ensure all directories in path exist.
 *
 * The Algorithm takes a pessimistic approach and works
 * top-down to ensure each directory in path exists.
 *
 * @path - absolute path to create
 * @mode - directory permission bits to use, e.g. 0774
 * (the mode is only evaluated on Unix systems!).
 *
 * @return 0 on success, -1 on error
 */
__GCC_DONT_EXPORT
int disk_mkPath(const char* path, int mode);


/**
 * disk_fileOrDirExists - Check whether the file named by the pathname
 * pointed to by 'path' is either a directory or a regular file.
 *
 * @return 1 if path points to a directory or a regular file, otherwise 0
 */
__GCC_DONT_EXPORT
int disk_fileOrDirExists(const char* path);


/**
 * disk_isRegularFile - Check whether the file named by the pathname
 * pointed to by 'path' is a regular file.
 *
 * @return 1 if path points to a regular file, otherwise 0
 */
__GCC_DONT_EXPORT
int disk_isRegularFile(const char* path);


/**
 * disk_removeFile - Attempt to remove the file named by the pathname
 * pointed to by 'path'.
 *
 * @return 0 on success, -1 on error
 */
__GCC_DONT_EXPORT
int disk_removeFile(const char* path);


#ifdef __cplusplus
}
#endif



#endif /* __DISK_UTIL_H__ */
