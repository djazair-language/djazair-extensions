#include "mysql_common.h"
#include <stddef.h>

static NativeMethod mysql_module_funcs[] = {
    {"init",              mysqlInitNative,              0},
    {"realConnect",       mysqlRealConnectNative,       6},
    {"setCharset",        mysqlSetCharsetNative,        2},
    {"close",             mysqlCloseNative,             1},
    {"error",             mysqlErrorNative,             1},
    {"errno",             mysqlErrnoNative,             1},
    {"ping",              mysqlPingNative,              1},
    {"stat",              mysqlStatNative,              1},
    {"protocolInfo",      mysqlGetProtoInfoNative,      1},
    {"threadId",          mysqlThreadIdNative,          1},
    {"info",              mysqlInfoNative,              1},
    {"clientInfo",        mysqlGetClientInfoNative,     0},
    {"serverInfo",        mysqlGetServerInfoNative,     1},
    {"hostInfo",          mysqlGetHostInfoNative,       1},
    {"query",             mysqlQueryNative,             2},
    {"affectedRows",      mysqlAffectedRowsNative,      1},
    {"insertId",          mysqlInsertIdNative,          1},
    {"escape",            mysqlEscapeStringNative,      2},
    {"storeResult",       mysqlStoreResultNative,       1},
    {"numRows",           mysqlNumRowsNative,           1},
    {"numFields",         mysqlNumFieldsNative,         1},
    {"columnNames",       mysqlGetColumnNamesNative,    1},
    {"fetchRow",          mysqlFetchRowNative,          1},
    {"fetchAssoc",        mysqlFetchRowAssocNative,     1},
    {"dataSeek",          mysqlDataSeekNative,          2},
    {"freeResult",        mysqlFreeResultNative,        1},
    {"beginTransaction",  mysqlBeginTransactionNative,  1},
    {"commit",            mysqlCommitNative,            1},
    {"rollback",          mysqlRollbackNative,          1},
    {NULL, NULL, 0}
};

DJAZAIR_EXTENSION(mysql, mysql_module_funcs)
