#include "mysql_common.h"
#include <stdlib.h>

DJAZAIR_FUNC(mysqlInitNative) {
    (void)argCount; (void)args;
    MYSQL *conn = mysql_init(NULL);
    if (!conn) return djazair_null();
    return djazair_new_resource(vm, conn, "MYSQLConn");
}

DJAZAIR_FUNC(mysqlRealConnectNative) {
    djazair_check_args(6, argCount);
    if (!djazair_is_resource(args[0]) || !djazair_is_string(args[1]) ||
        !djazair_is_string(args[2]) || !djazair_is_string(args[3]) ||
        !djazair_is_string(args[4]) || !djazair_is_number(args[5]))
        return djazair_bool(false);

    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_bool(false);

    const char *host = djazair_get_str(args, 1);
    const char *user = djazair_get_str(args, 2);
    const char *pass = djazair_get_str(args, 3);
    const char *db   = djazair_get_str(args, 4);
    int port         = (int)djazair_get_num(args, 5);

    return djazair_bool(mysql_real_connect(conn, host, user, pass, db, port, NULL, 0) != NULL);
}

DJAZAIR_FUNC(mysqlSetCharsetNative) {
    djazair_check_args(2, argCount);
    if (!djazair_is_resource(args[0]) || !djazair_is_string(args[1]))
        return djazair_bool(false);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_bool(false);
    return djazair_bool(mysql_set_character_set(conn, djazair_get_str(args, 1)) == 0);
}

DJAZAIR_FUNC(mysqlCloseNative) {
    djazair_check_args(1, argCount);
    if (djazair_is_resource(args[0])) {
        MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
        if (conn) {
            mysql_close(conn);
            AS_RESOURCE(args[0])->ptr = NULL;
        }
    }
    return djazair_null();
}

DJAZAIR_FUNC(mysqlErrorNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_str(vm, "Invalid connection");
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_str(vm, "Connection closed");
    return djazair_str(vm, mysql_error(conn));
}

DJAZAIR_FUNC(mysqlErrnoNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_num(0);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_num(0);
    return djazair_num((double)mysql_errno(conn));
}

DJAZAIR_FUNC(mysqlPingNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_bool(false);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_bool(false);
    return djazair_bool(mysql_ping(conn) == 0);
}

DJAZAIR_FUNC(mysqlStatNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_null();
    const char *stat = mysql_stat(conn);
    return stat ? djazair_str(vm, stat) : djazair_null();
}

DJAZAIR_FUNC(mysqlGetProtoInfoNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_num(0);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_num(0);
    return djazair_num((double)mysql_get_proto_info(conn));
}

DJAZAIR_FUNC(mysqlThreadIdNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_num(0);
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_num(0);
    return djazair_num((double)mysql_thread_id(conn));
}

DJAZAIR_FUNC(mysqlInfoNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_null();
    const char *info = mysql_info(conn);
    return info ? djazair_str(vm, info) : djazair_null();
}

DJAZAIR_FUNC(mysqlGetClientInfoNative) {
    (void)argCount; (void)args;
    return djazair_str(vm, mysql_get_client_info());
}

DJAZAIR_FUNC(mysqlGetServerInfoNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_null();
    return djazair_str(vm, mysql_get_server_info(conn));
}

DJAZAIR_FUNC(mysqlGetHostInfoNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_null();
    return djazair_str(vm, mysql_get_host_info(conn));
}
