#include "mysql_common.h"
#include <stdlib.h>

DJAZAIR_FUNC(mysqlQueryNative) {
    djazair_check_args(2, argCount);
    if (!djazair_is_resource(args[0]) || !djazair_is_string(args[1]))
        return djazair_bool(false);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_bool(false);

    size_t sqlLen;
    const char *sql = djazair_get_str_l(args, 1, &sqlLen);
    return djazair_bool(mysql_real_query(conn, sql, (unsigned long)sqlLen) == 0);
}

DJAZAIR_FUNC(mysqlAffectedRowsNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_num(0);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_num(0);
    return djazair_num((double)mysql_affected_rows(conn));
}

DJAZAIR_FUNC(mysqlInsertIdNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_num(0);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_num(0);
    return djazair_num((double)mysql_insert_id(conn));
}

DJAZAIR_FUNC(mysqlEscapeStringNative) {
    djazair_check_args(2, argCount);
    if (!djazair_is_resource(args[0]) || !djazair_is_string(args[1]))
        return args[1];
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return args[1];

    size_t strLen;
    const char *str = djazair_get_str_l(args, 1, &strLen);
    char *escaped = (char *)malloc(strLen * 2 + 1);
    if (!escaped) return args[1];

    unsigned long escaped_len = mysql_real_escape_string(conn, escaped, str, (unsigned long)strLen);
    djazair_value result = djazair_str_l(vm, escaped, (size_t)escaped_len);
    free(escaped);
    return result;
}

DJAZAIR_FUNC(mysqlBeginTransactionNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_bool(false);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_bool(false);
    return djazair_bool(mysql_autocommit(conn, 0) == 0);
}

DJAZAIR_FUNC(mysqlCommitNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_bool(false);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_bool(false);
    bool ok = (mysql_commit(conn) == 0);
    mysql_autocommit(conn, 1);
    return djazair_bool(ok);
}

DJAZAIR_FUNC(mysqlRollbackNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_bool(false);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_bool(false);
    bool ok = (mysql_rollback(conn) == 0);
    mysql_autocommit(conn, 1);
    return djazair_bool(ok);
}
