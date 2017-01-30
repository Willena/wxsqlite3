// Enable SQLite debug assertions if requested
#ifndef SQLITE_DEBUG
#if defined(SQLITE_ENABLE_DEBUG) && (SQLITE_ENABLE_DEBUG == 1)
#define SQLITE_DEBUG
#endif
#endif

// To enable the extension functions define SQLITE_ENABLE_EXTFUNC on compiling this module
// To enable the reading CSV files define SQLITE_ENABLE_CSV on compiling this module
#if defined(SQLITE_ENABLE_EXTFUNC) || defined(SQLITE_ENABLE_CSV)
#define sqlite3_open    sqlite3_open_internal
#define sqlite3_open16  sqlite3_open16_internal
#define sqlite3_open_v2 sqlite3_open_v2_internal
#endif

// Enable the user authentication feature
#ifndef SQLITE_USER_AUTHENTICATION
#define SQLITE_USER_AUTHENTICATION 1
#endif

#include "sqlite3.c"
#ifdef SQLITE_USER_AUTHENTICATION
#include "sha2.h"
#include "sha2.c"
#include "userauth.c"
#endif

#if defined(SQLITE_ENABLE_EXTFUNC) || defined(SQLITE_ENABLE_CSV)
#undef sqlite3_open
#undef sqlite3_open16
#undef sqlite3_open_v2
#endif

#ifndef SQLITE_OMIT_DISKIO

#ifdef SQLITE_HAS_CODEC

/*
** Get the codec argument for this pager
*/

void* mySqlite3PagerGetCodec(
  Pager *pPager
){
#if (SQLITE_VERSION_NUMBER >= 3006016)
  return sqlite3PagerGetCodec(pPager);
#else
  return (pPager->xCodec) ? pPager->pCodecArg : NULL;
#endif
}

/*
** Set the codec argument for this pager
*/

void mySqlite3PagerSetCodec(
  Pager *pPager,
  void *(*xCodec)(void*,void*,Pgno,int),
  void (*xCodecSizeChng)(void*,int,int),
  void (*xCodecFree)(void*),
  void *pCodec
){
  sqlite3PagerSetCodec(pPager, xCodec, xCodecSizeChng, xCodecFree, pCodec);
}

#include "rijndael.c"
#include "codec.c"
#include "codecext.c"

#endif

#endif

/* Extension functions */
#ifdef SQLITE_ENABLE_EXTFUNC
#include "extensionfunctions.c"
#endif

/* CSV import */
#ifdef SQLITE_ENABLE_CSV
#include "csv.c"
#endif

#if defined(SQLITE_ENABLE_EXTFUNC) || defined(SQLITE_ENABLE_CSV)

SQLITE_API int sqlite3_open(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
)
{
  int ret = sqlite3_open_internal(filename, ppDb);
  if (ret == 0)
  {
#ifdef SQLITE_ENABLE_EXTFUNC
    RegisterExtensionFunctions(*ppDb);
#endif
#ifdef SQLITE_ENABLE_CSV
    sqlite3_csv_init(*ppDb, NULL, NULL);
#endif
  }
  return ret;
}

SQLITE_API int sqlite3_open16(
  const void *filename,   /* Database filename (UTF-16) */
  sqlite3 **ppDb          /* OUT: SQLite db handle */
)
{
  int ret = sqlite3_open16_internal(filename, ppDb);
  if (ret == 0)
  {
#ifdef SQLITE_ENABLE_EXTFUNC
    RegisterExtensionFunctions(*ppDb);
#endif
#ifdef SQLITE_ENABLE_CSV
    sqlite3_csv_init(*ppDb, NULL, NULL);
#endif
  }
  return ret;
}

SQLITE_API int sqlite3_open_v2(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb,         /* OUT: SQLite db handle */
  int flags,              /* Flags */
  const char *zVfs        /* Name of VFS module to use */
)
{
  int ret = sqlite3_open_v2_internal(filename, ppDb, flags, zVfs);
  if (ret == 0)
  {
#ifdef SQLITE_ENABLE_EXTFUNC
    RegisterExtensionFunctions(*ppDb);
#endif
#ifdef SQLITE_ENABLE_CSV
    sqlite3_csv_init(*ppDb, NULL, NULL);
#endif
  }
  return ret;
}

#endif
