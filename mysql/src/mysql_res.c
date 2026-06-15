#include "mysql_common.h"
#include <string.h>

DJAZAIR_FUNC(mysqlStoreResultNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    MYSQL *conn = (MYSQL *)djazair_get_resource(vm, args[0]);
    if (!conn) return djazair_null();

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) return djazair_null();
    return djazair_new_resource(vm, res, "MYSQLRes");
}

DJAZAIR_FUNC(mysqlNumRowsNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_num(0);
    MYSQL_RES *res = (MYSQL_RES *)djazair_get_resource(vm, args[0]);
    if (!res) return djazair_num(0);
    return djazair_num((double)mysql_num_rows(res));
}

DJAZAIR_FUNC(mysqlNumFieldsNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_num(0);
    MYSQL_RES *res = (MYSQL_RES *)djazair_get_resource(vm, args[0]);
    if (!res) return djazair_num(0);
    return djazair_num((double)mysql_num_fields(res));
}

DJAZAIR_FUNC(mysqlGetColumnNamesNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    MYSQL_RES *res = (MYSQL_RES *)djazair_get_resource(vm, args[0]);
    if (!res) return djazair_null();

    unsigned int nfields = mysql_num_fields(res);
    MYSQL_FIELD *fields  = mysql_fetch_fields(res);

    djazair_value arr = djazair_new_array(vm);
    djazair_push(vm, arr);
    for (unsigned int i = 0; i < nfields; i++) {
        djazair_array_push(vm, arr, djazair_str(vm, fields[i].name));
    }
    return djazair_pop(vm);
}

DJAZAIR_FUNC(mysqlFetchRowNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    MYSQL_RES *res = (MYSQL_RES *)djazair_get_resource(vm, args[0]);
    if (!res) return djazair_null();

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) return djazair_null();

    unsigned int nfields = mysql_num_fields(res);
    djazair_value arr = djazair_new_array(vm);
    djazair_push(vm, arr);
    for (unsigned int i = 0; i < nfields; i++) {
        djazair_array_push(vm, arr, row[i] ? djazair_str(vm, row[i]) : djazair_null());
    }
    return djazair_pop(vm);
}

DJAZAIR_FUNC(mysqlFetchRowAssocNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    MYSQL_RES *res = (MYSQL_RES *)djazair_get_resource(vm, args[0]);
    if (!res) return djazair_null();

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row) return djazair_null();

    unsigned int nfields = mysql_num_fields(res);
    MYSQL_FIELD *fields  = mysql_fetch_fields(res);

    djazair_value map = djazair_new_map(vm);
    djazair_push(vm, map);
    for (unsigned int i = 0; i < nfields; i++) {
        djazair_map_set(vm, map, djazair_str(vm, fields[i].name), row[i] ? djazair_str(vm, row[i]) : djazair_null());
    }
    return djazair_pop(vm);
}

DJAZAIR_FUNC(mysqlDataSeekNative) {
    djazair_check_args(2, argCount);
    if (!djazair_is_resource(args[0]) || !djazair_is_number(args[1]))
        return djazair_null();
    MYSQL_RES *res = (MYSQL_RES *)djazair_get_resource(vm, args[0]);
    if (!res) return djazair_null();
    mysql_data_seek(res, (my_ulonglong)djazair_get_num(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(mysqlFreeResultNative) {
    djazair_check_args(1, argCount);
    if (djazair_is_resource(args[0])) {
        MYSQL_RES *res = (MYSQL_RES *)djazair_get_resource(vm, args[0]);
        if (res) {
            mysql_free_result(res);
            AS_RESOURCE(args[0])->ptr = NULL;
        }
    }
    return djazair_null();
}
