#ifndef DJAZAIR_MYSQL_COMMON_H
#define DJAZAIR_MYSQL_COMMON_H

#include "djazair_api.h"
#include <mysql.h>

// Connection Operations
DJAZAIR_FUNC(mysqlInitNative);
DJAZAIR_FUNC(mysqlRealConnectNative);
DJAZAIR_FUNC(mysqlSetCharsetNative);
DJAZAIR_FUNC(mysqlCloseNative);
DJAZAIR_FUNC(mysqlErrorNative);
DJAZAIR_FUNC(mysqlErrnoNative);
DJAZAIR_FUNC(mysqlPingNative);
DJAZAIR_FUNC(mysqlStatNative);
DJAZAIR_FUNC(mysqlGetProtoInfoNative);
DJAZAIR_FUNC(mysqlThreadIdNative);
DJAZAIR_FUNC(mysqlInfoNative);
DJAZAIR_FUNC(mysqlGetClientInfoNative);
DJAZAIR_FUNC(mysqlGetServerInfoNative);
DJAZAIR_FUNC(mysqlGetHostInfoNative);

// Query / Transaction Operations
DJAZAIR_FUNC(mysqlQueryNative);
DJAZAIR_FUNC(mysqlAffectedRowsNative);
DJAZAIR_FUNC(mysqlInsertIdNative);
DJAZAIR_FUNC(mysqlEscapeStringNative);
DJAZAIR_FUNC(mysqlBeginTransactionNative);
DJAZAIR_FUNC(mysqlCommitNative);
DJAZAIR_FUNC(mysqlRollbackNative);

// Result Operations
DJAZAIR_FUNC(mysqlStoreResultNative);
DJAZAIR_FUNC(mysqlNumRowsNative);
DJAZAIR_FUNC(mysqlNumFieldsNative);
DJAZAIR_FUNC(mysqlGetColumnNamesNative);
DJAZAIR_FUNC(mysqlFetchRowNative);
DJAZAIR_FUNC(mysqlFetchRowAssocNative);
DJAZAIR_FUNC(mysqlDataSeekNative);
DJAZAIR_FUNC(mysqlFreeResultNative);

#endif // DJAZAIR_MYSQL_COMMON_H
