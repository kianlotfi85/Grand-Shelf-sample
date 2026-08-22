/*
 * Trimmed declarations for the subset of the SQLite3 C API used by
 * study-planner. This file is only on the include path when the real
 * sqlite3.h (libsqlite3-dev) is missing; the ABI is stable, so linking against
 * an installed libsqlite3 runtime works. Install libsqlite3-dev to use the
 * upstream header instead.
 */
#ifndef STUDY_PLANNER_VENDORED_SQLITE3_H
#define STUDY_PLANNER_VENDORED_SQLITE3_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sqlite3 sqlite3;
typedef struct sqlite3_stmt sqlite3_stmt;
typedef long long int sqlite3_int64;
typedef void (*sqlite3_destructor_type)(void *);

#define SQLITE_OK 0
#define SQLITE_ERROR 1
#define SQLITE_BUSY 5
#define SQLITE_CONSTRAINT 19
#define SQLITE_MISUSE 21
#define SQLITE_ROW 100
#define SQLITE_DONE 101

#define SQLITE_INTEGER 1
#define SQLITE_FLOAT 2
#define SQLITE_TEXT 3
#define SQLITE_BLOB 4
#define SQLITE_NULL 5

#define SQLITE_OPEN_READONLY 0x00000001
#define SQLITE_OPEN_READWRITE 0x00000002
#define SQLITE_OPEN_CREATE 0x00000004
#define SQLITE_OPEN_URI 0x00000040
#define SQLITE_OPEN_NOMUTEX 0x00008000
#define SQLITE_OPEN_FULLMUTEX 0x00010000

#define SQLITE_STATIC ((sqlite3_destructor_type)0)
#define SQLITE_TRANSIENT ((sqlite3_destructor_type)-1)

const char *sqlite3_libversion(void);
int sqlite3_threadsafe(void);

int sqlite3_open_v2(const char *filename, sqlite3 **db, int flags, const char *vfs);
int sqlite3_close_v2(sqlite3 *db);
int sqlite3_busy_timeout(sqlite3 *db, int ms);
int sqlite3_exec(sqlite3 *db, const char *sql,
                 int (*callback)(void *, int, char **, char **), void *arg,
                 char **errmsg);
const char *sqlite3_errmsg(sqlite3 *db);
int sqlite3_errcode(sqlite3 *db);
const char *sqlite3_errstr(int code);
void sqlite3_free(void *p);
sqlite3_int64 sqlite3_last_insert_rowid(sqlite3 *db);
int sqlite3_changes(sqlite3 *db);

int sqlite3_prepare_v2(sqlite3 *db, const char *sql, int nbyte,
                       sqlite3_stmt **stmt, const char **tail);
int sqlite3_step(sqlite3_stmt *stmt);
int sqlite3_reset(sqlite3_stmt *stmt);
int sqlite3_clear_bindings(sqlite3_stmt *stmt);
int sqlite3_finalize(sqlite3_stmt *stmt);
const char *sqlite3_sql(sqlite3_stmt *stmt);

int sqlite3_bind_int(sqlite3_stmt *stmt, int index, int value);
int sqlite3_bind_int64(sqlite3_stmt *stmt, int index, sqlite3_int64 value);
int sqlite3_bind_double(sqlite3_stmt *stmt, int index, double value);
int sqlite3_bind_text(sqlite3_stmt *stmt, int index, const char *value, int nbyte,
                      sqlite3_destructor_type destructor);
int sqlite3_bind_null(sqlite3_stmt *stmt, int index);
int sqlite3_bind_parameter_index(sqlite3_stmt *stmt, const char *name);
int sqlite3_bind_parameter_count(sqlite3_stmt *stmt);

int sqlite3_column_count(sqlite3_stmt *stmt);
const char *sqlite3_column_name(sqlite3_stmt *stmt, int col);
int sqlite3_column_type(sqlite3_stmt *stmt, int col);
int sqlite3_column_int(sqlite3_stmt *stmt, int col);
sqlite3_int64 sqlite3_column_int64(sqlite3_stmt *stmt, int col);
double sqlite3_column_double(sqlite3_stmt *stmt, int col);
const unsigned char *sqlite3_column_text(sqlite3_stmt *stmt, int col);
int sqlite3_column_bytes(sqlite3_stmt *stmt, int col);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* STUDY_PLANNER_VENDORED_SQLITE3_H */
