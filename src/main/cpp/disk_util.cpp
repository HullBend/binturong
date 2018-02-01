
#include "disk_util.h"

//#include <string> // comes from stdafx.h via mkpath.h

#include <string.h>
#include <stdio.h> // remove

// these headers also exist on Win
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>




#if defined (_WIN64) || defined (_WIN32)

//#include <windows.h> // CreateDirectoryA (comes from stdafx.h via mkpath.h)
#include "std_stringutil.h" // stringReplaceAll

typedef struct _stat Stat;
#define STAT_PORTABLE(path, pStatus) _stat((path), (pStatus))

#define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#define S_ISREG(mode) (((mode) & S_IFMT) == S_IFREG)

#else /* Unix / Linux */

typedef struct stat Stat;
#define STAT_PORTABLE(path, pStatus) stat((path), (pStatus))

#endif /* _WIN64 || _WIN32 */



static bool isWin() {
#if defined (_WIN64) || defined (_WIN32)
    return true;
#else
    return false;
#endif
}


static bool makeDir_(const char* path, int mode) {

#if defined (_WIN64) || defined (_WIN32)

    (void) mode; // unused

    if (!CreateDirectoryA(path, NULL)) {
        /* don't treat it as an error if directory already exists */
        return (GetLastError() == ERROR_ALREADY_EXISTS);
    }
    return true;

#else /* Unix / Linux */

    return (mkdir(path, mode) == 0);

#endif

}


static int makeDir(const char* path, int mode) {

    if (isWin() && strlen(path) == 2 && path[1] == ':') {
        /* it's a Windows drive letter */
        return 0;
    }

    Stat st;
    int status = 0;

    if (STAT_PORTABLE(path, &st) != 0) {
        /* Directory does not exist */
        if (!makeDir_(path, mode)) {
            status = -1;
        }
    } else if (!S_ISDIR(st.st_mode)) {
        errno = ENOTDIR;
        status = -1;
    }
    return status;
}


static int mkpath_(const char* path, int mode) {

    const std::string pathCopy(path);
    const char* copy = pathCopy.c_str();
    char* pp = const_cast<char*>(copy); /* Ouch! */
    char* sp = NULL;
    int status = 0;

    while (status == 0 && (sp = strchr(pp, '/')) != NULL) {
        if (sp != pp) {
            *sp = '\0';
            status = makeDir(copy, mode);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0) {
        status = makeDir(path, mode);
    }

    return status;
}


/**
 * disk_mkPath - Ensure all directories in path exist.
 *
 * The Algorithm takes a pessimistic approach and works
 * top-down to ensure each directory in path exists.
 */
int disk_mkPath(const char* path, int mode) {

#if defined (_WIN64) || defined (_WIN32)

    (void) mode; /* unused */

    if (path != NULL && (strchr(path, '\\') != NULL)) {
        std::string copy(path);
        const std::string& replaced = stringReplaceAll(copy, "\\", "/");
        return mkpath_(replaced.c_str(), 0);
    } else {
        return mkpath_(path, 0);
    }

#else /* Unix / Linux */

    return mkpath_(path, mode);

#endif

}


int disk_fileOrDirExists(const char* path) {
    Stat st;
    if (STAT_PORTABLE(path, &st) == 0) {
        if (S_ISDIR(st.st_mode) || S_ISREG(st.st_mode)) {
            // a directory or a regular file
            return 1;
        }
    }
    return 0;
}


int disk_isRegularFile(const char* path) {
    Stat st;
    if (STAT_PORTABLE(path, &st) == 0 && S_ISREG(st.st_mode)) {
        // it's a regular file
        return 1;
    }
    return 0;
}


int disk_removeFile(const char* path) {
    return remove(path);
}
