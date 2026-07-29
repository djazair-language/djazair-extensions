/**
 * [ Djazair Programming Language Project ]
 * Developer: Harizi Riyadh (hariziriyadh@gmail.com)
 *
 * qt_djazair.cpp — Enterprise Djazair VM Native Bindings for Qt Extension
 * =================================================================
 * Exposes native C++ Qt operations to Djazair scripts via djazair_api.h.
 */

#include "qtWrapper.h"

extern "C" {
#include "djazair_api.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <string>

/* Resource finalizer callback for parentless Qt objects to prevent memory leaks */
static void qtResourceFinalizer(void* ptr) {
    if (!ptr) return;
    qt_object_delete(ptr);
}

static void* get_qt_handle(djazairVM* vm, djazair_args args, int index) {
    if (!IS_RESOURCE(args[index])) {
        runtimeError(vm, "TypeError: Argument %d must be a resource.", index + 1);
        return NULL;
    }
    return djazair_get_resource(vm, args[index]);
}

/* ============================================================
 * Application Life Cycle & Theme
 * ============================================================ */
DJAZAIR_FUNC(qtAppCreateNative) {
    QtAppHandle app = qt_app_create(0, NULL);
    return djazair_new_resource_with_finalizer(vm, app, "QtApp", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtAppExecNative) {
    djazair_check_args(1, argCount);
    QtAppHandle app = (QtAppHandle)get_qt_handle(vm, args, 0);
    if (!app) return NULL_VAL;
    int status = qt_app_exec(app);
    return djazair_num(status);
}

DJAZAIR_FUNC(qtAppProcessEventsNative) {
    djazair_check_args(1, argCount);
    QtAppHandle app = (QtAppHandle)get_qt_handle(vm, args, 0);
    if (!app) return NULL_VAL;
    qt_app_process_events(app);
    return djazair_null();
}

DJAZAIR_FUNC(qtAppQuitNative) {
    djazair_check_args(1, argCount);
    QtAppHandle app = (QtAppHandle)get_qt_handle(vm, args, 0);
    if (!app) return NULL_VAL;
    qt_app_quit(app);
    return djazair_null();
}

DJAZAIR_FUNC(qtAppSetStyleNative) {
    djazair_check_args(1, argCount);
    djazair_check_str(0);
    qt_app_set_style(djazair_get_str(args, 0));
    return djazair_null();
}

DJAZAIR_FUNC(qtAppDestroyNative) {
    djazair_check_args(1, argCount);
    QtAppHandle app = (QtAppHandle)get_qt_handle(vm, args, 0);
    if (!app) return NULL_VAL;
    qt_app_destroy(app);
    return djazair_null();
}

/* ============================================================
 * Dynamic UI Loader (QUiLoader)
 * ============================================================ */
DJAZAIR_FUNC(qtLoadUiNative) {
    djazair_check_min_args(1, argCount);
    djazair_check_str(0);
    const char* filepath = djazair_get_str(args, 0);
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle w = qt_uiloader_load_file(filepath, parent);
    if (!w) return NULL_VAL;
    return djazair_new_resource_with_finalizer(vm, w, "QWidget", qtResourceFinalizer);
}

/* ============================================================
 * Multimedia Engine (QMediaPlayer)
 * ============================================================ */
DJAZAIR_FUNC(qtMediaPlayerCreateNative) {
    QtMediaPlayerHandle player = qt_mediaplayer_create();
    return djazair_new_resource_with_finalizer(vm, player, "QMediaPlayer", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtMediaPlayerSetMediaNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtMediaPlayerHandle player = (QtMediaPlayerHandle)get_qt_handle(vm, args, 0);
    if (!player) return NULL_VAL;
    qt_mediaplayer_set_media(player, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtMediaPlayerPlayNative) {
    djazair_check_args(1, argCount);
    QtMediaPlayerHandle player = (QtMediaPlayerHandle)get_qt_handle(vm, args, 0);
    if (!player) return NULL_VAL;
    qt_mediaplayer_play(player);
    return djazair_null();
}

DJAZAIR_FUNC(qtMediaPlayerPauseNative) {
    djazair_check_args(1, argCount);
    QtMediaPlayerHandle player = (QtMediaPlayerHandle)get_qt_handle(vm, args, 0);
    if (!player) return NULL_VAL;
    qt_mediaplayer_pause(player);
    return djazair_null();
}

DJAZAIR_FUNC(qtMediaPlayerStopNative) {
    djazair_check_args(1, argCount);
    QtMediaPlayerHandle player = (QtMediaPlayerHandle)get_qt_handle(vm, args, 0);
    if (!player) return NULL_VAL;
    qt_mediaplayer_stop(player);
    return djazair_null();
}

DJAZAIR_FUNC(qtVideoWidgetCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle v = qt_videowidget_create(parent);
    return djazair_new_resource_with_finalizer(vm, v, "QVideoWidget", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtMediaPlayerSetVideoOutputNative) {
    djazair_check_args(2, argCount);
    QtMediaPlayerHandle player = (QtMediaPlayerHandle)get_qt_handle(vm, args, 0);
    QtWidgetHandle v = (QtWidgetHandle)get_qt_handle(vm, args, 1);
    if (!player || !v) return NULL_VAL;
    qt_mediaplayer_set_video_output(player, v);
    return djazair_null();
}

/* ============================================================
 * Base QWidget Operations & Properties
 * ============================================================ */
DJAZAIR_FUNC(qtWidgetCreateNative) {
    QtWidgetHandle parent = NULL;
    if (argCount > 0 && djazair_is_resource(args[0])) {
        parent = (QtWidgetHandle)djazair_get_resource(vm, args[0]);
    }
    QtWidgetHandle w = qt_widget_create(parent);
    return djazair_new_resource_with_finalizer(vm, w, "QWidget", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtWidgetShowNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    qt_widget_show(w);
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetHideNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    qt_widget_hide(w);
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetSetTitleNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    qt_widget_set_title(w, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetSetGeometryNative) {
    djazair_check_args(5, argCount);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    int x = (int)djazair_get_num(args, 1);
    int y = (int)djazair_get_num(args, 2);
    int width = (int)djazair_get_num(args, 3);
    int height = (int)djazair_get_num(args, 4);
    qt_widget_set_geometry(w, x, y, width, height);
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetResizeNative) {
    djazair_check_args(3, argCount);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    int width = (int)djazair_get_num(args, 1);
    int height = (int)djazair_get_num(args, 2);
    qt_widget_resize(w, width, height);
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetSetLayoutNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    QtLayoutHandle l = (QtLayoutHandle)get_qt_handle(vm, args, 1);
    if (!l) return NULL_VAL;
    qt_widget_set_layout(w, l);
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetSetStyleSheetNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    qt_widget_set_style_sheet(w, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetSetEnabledNative) {
    djazair_check_args(2, argCount);
    djazair_check_bool(1);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    qt_widget_set_enabled(w, djazair_get_bool(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetSetToolTipNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    qt_widget_set_tooltip(w, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetUpdateNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    qt_widget_update(w);
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetDestroyNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    qt_widget_destroy(w);
    return djazair_null();
}

/* ============================================================
 * QMainWindow, Menus, ToolBar, StatusBar
 * ============================================================ */
DJAZAIR_FUNC(qtMainWindowCreateNative) {
    QtMainWindowHandle main_win = qt_mainwindow_create();
    return djazair_new_resource_with_finalizer(vm, main_win, "QMainWindow", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtMainWindowSetCentralWidgetNative) {
    djazair_check_args(2, argCount);
    QtMainWindowHandle main_win = (QtMainWindowHandle)get_qt_handle(vm, args, 0);
    if (!main_win) return NULL_VAL;
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 1);
    if (!w) return NULL_VAL;
    qt_mainwindow_set_central_widget(main_win, w);
    return djazair_null();
}

DJAZAIR_FUNC(qtMainWindowSetTitleNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtMainWindowHandle main_win = (QtMainWindowHandle)get_qt_handle(vm, args, 0);
    if (!main_win) return NULL_VAL;
    qt_mainwindow_set_title(main_win, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtMainWindowResizeNative) {
    djazair_check_args(3, argCount);
    QtMainWindowHandle main_win = (QtMainWindowHandle)get_qt_handle(vm, args, 0);
    if (!main_win) return NULL_VAL;
    int width = (int)djazair_get_num(args, 1);
    int height = (int)djazair_get_num(args, 2);
    qt_mainwindow_resize(main_win, width, height);
    return djazair_null();
}

DJAZAIR_FUNC(qtMainWindowShowNative) {
    djazair_check_args(1, argCount);
    QtMainWindowHandle main_win = (QtMainWindowHandle)get_qt_handle(vm, args, 0);
    if (!main_win) return NULL_VAL;
    qt_mainwindow_show(main_win);
    return djazair_null();
}

DJAZAIR_FUNC(qtMainWindowAddMenuNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtMainWindowHandle main_win = (QtMainWindowHandle)get_qt_handle(vm, args, 0);
    if (!main_win) return NULL_VAL;
    QtWidgetHandle menu = qt_mainwindow_add_menu(main_win, djazair_get_str(args, 1));
    return djazair_new_resource_with_finalizer(vm, menu, "QMenu", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtMenuAddSubmenuNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtMenuHandle menu = (QtMenuHandle)get_qt_handle(vm, args, 0);
    if (!menu) return NULL_VAL;
    QtMenuHandle sub = qt_menu_add_submenu(menu, djazair_get_str(args, 1));
    return djazair_new_resource_with_finalizer(vm, sub, "QMenu", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtMenuAddActionNative) {
    djazair_check_min_args(2, argCount);
    djazair_check_str(1);
    QtMenuHandle menu = (QtMenuHandle)get_qt_handle(vm, args, 0);
    if (!menu) return NULL_VAL;
    QtActionHandle action = qt_menu_add_action(menu, djazair_get_str(args, 1), NULL, NULL);
    return djazair_new_resource_with_finalizer(vm, action, "QAction", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtMenuAddSeparatorNative) {
    djazair_check_args(1, argCount);
    QtMenuHandle menu = (QtMenuHandle)get_qt_handle(vm, args, 0);
    if (!menu) return NULL_VAL;
    qt_menu_add_separator(menu);
    return djazair_null();
}

DJAZAIR_FUNC(qtMainWindowAddToolBarNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtMainWindowHandle main_win = (QtMainWindowHandle)get_qt_handle(vm, args, 0);
    if (!main_win) return NULL_VAL;
    QtWidgetHandle bar = qt_mainwindow_add_toolbar(main_win, djazair_get_str(args, 1));
    return djazair_new_resource_with_finalizer(vm, bar, "QToolBar", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtToolBarAddActionNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle bar = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!bar) return NULL_VAL;
    qt_toolbar_add_action(bar, djazair_get_str(args, 1), NULL, NULL);
    return djazair_null();
}

DJAZAIR_FUNC(qtMainWindowSetStatusMessageNative) {
    djazair_check_args(3, argCount);
    djazair_check_str(1);
    QtMainWindowHandle main_win = (QtMainWindowHandle)get_qt_handle(vm, args, 0);
    if (!main_win) return NULL_VAL;
    const char* msg = djazair_get_str(args, 1);
    int timeout = (int)djazair_get_num(args, 2);
    qt_mainwindow_set_status_message(main_win, msg, timeout);
    return djazair_null();
}

/* ============================================================
 * Basic Controls
 * ============================================================ */
DJAZAIR_FUNC(qtButtonCreateNative) {
    const char* text = "";
    QtWidgetHandle parent = NULL;
    if (argCount > 0 && djazair_is_string(args[0])) text = djazair_get_str(args, 0);
    if (argCount > 1 && djazair_is_resource(args[1])) parent = (QtWidgetHandle)djazair_get_resource(vm, args[1]);
    QtWidgetHandle btn = qt_button_create(text, parent);
    return djazair_new_resource_with_finalizer(vm, btn, "QPushButton", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtButtonSetTextNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle btn = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!btn) return NULL_VAL;
    qt_button_set_text(btn, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtLabelCreateNative) {
    const char* text = "";
    QtWidgetHandle parent = NULL;
    if (argCount > 0 && djazair_is_string(args[0])) text = djazair_get_str(args, 0);
    if (argCount > 1 && djazair_is_resource(args[1])) parent = (QtWidgetHandle)djazair_get_resource(vm, args[1]);
    QtWidgetHandle label = qt_label_create(text, parent);
    return djazair_new_resource_with_finalizer(vm, label, "QLabel", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtLabelSetTextNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle label = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!label) return NULL_VAL;
    qt_label_set_text(label, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtLabelGetTextNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle label = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!label) return NULL_VAL;
    return djazair_str(vm, qt_label_get_text(label));
}

DJAZAIR_FUNC(qtLabelSetAlignmentNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle label = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!label) return NULL_VAL;
    int align = (int)djazair_get_num(args, 1);
    qt_label_set_alignment(label, align);
    return djazair_null();
}

DJAZAIR_FUNC(qtLabelSetPixmapNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle label = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    QtPixmapHandle pix = (QtPixmapHandle)get_qt_handle(vm, args, 1);
    if (!label || !pix) return NULL_VAL;
    qt_label_set_pixmap(label, pix);
    return djazair_null();
}

DJAZAIR_FUNC(qtLineEditCreateNative) {
    const char* text = "";
    QtWidgetHandle parent = NULL;
    if (argCount > 0 && djazair_is_string(args[0])) text = djazair_get_str(args, 0);
    if (argCount > 1 && djazair_is_resource(args[1])) parent = (QtWidgetHandle)djazair_get_resource(vm, args[1]);
    QtWidgetHandle edit = qt_lineedit_create(text, parent);
    return djazair_new_resource_with_finalizer(vm, edit, "QLineEdit", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtLineEditSetTextNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    qt_lineedit_set_text(edit, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtLineEditGetTextNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    return djazair_str(vm, qt_lineedit_get_text(edit));
}

DJAZAIR_FUNC(qtLineEditSetPlaceholderNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    qt_lineedit_set_placeholder(edit, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtLineEditSetEchoModeNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    int mode = (int)djazair_get_num(args, 1);
    qt_lineedit_set_echo_mode(edit, mode);
    return djazair_null();
}

DJAZAIR_FUNC(qtTextEditCreateNative) {
    const char* text = "";
    QtWidgetHandle parent = NULL;
    if (argCount > 0 && djazair_is_string(args[0])) text = djazair_get_str(args, 0);
    if (argCount > 1 && djazair_is_resource(args[1])) parent = (QtWidgetHandle)djazair_get_resource(vm, args[1]);
    QtWidgetHandle edit = qt_textedit_create(text, parent);
    return djazair_new_resource_with_finalizer(vm, edit, "QTextEdit", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtTextEditSetTextNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    qt_textedit_set_text(edit, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtTextEditGetTextNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    return djazair_str(vm, qt_textedit_get_text(edit));
}

DJAZAIR_FUNC(qtTextEditClearNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    qt_textedit_clear(edit);
    return djazair_null();
}

DJAZAIR_FUNC(qtTextEditAppendNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    qt_textedit_append(edit, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtCheckBoxCreateNative) {
    const char* text = (argCount > 0 && djazair_is_string(args[0])) ? djazair_get_str(args, 0) : "";
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle cb = qt_checkbox_create(text, parent);
    return djazair_new_resource_with_finalizer(vm, cb, "QCheckBox", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtCheckBoxIsCheckedNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle cb = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!cb) return NULL_VAL;
    return djazair_bool(qt_checkbox_is_checked(cb));
}

DJAZAIR_FUNC(qtCheckBoxSetCheckedNative) {
    djazair_check_args(2, argCount);
    djazair_check_bool(1);
    QtWidgetHandle cb = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!cb) return NULL_VAL;
    qt_checkbox_set_checked(cb, djazair_get_bool(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtRadioButtonCreateNative) {
    const char* text = (argCount > 0 && djazair_is_string(args[0])) ? djazair_get_str(args, 0) : "";
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle rb = qt_radiobutton_create(text, parent);
    return djazair_new_resource_with_finalizer(vm, rb, "QRadioButton", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtRadioButtonSetCheckedNative) {
    djazair_check_args(2, argCount);
    djazair_check_bool(1);
    QtWidgetHandle rb = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!rb) return NULL_VAL;
    qt_radiobutton_set_checked(rb, djazair_get_bool(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtRadioButtonIsCheckedNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle rb = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!rb) return NULL_VAL;
    return djazair_bool(qt_radiobutton_is_checked(rb));
}

DJAZAIR_FUNC(qtComboBoxCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle combo = qt_combobox_create(parent);
    return djazair_new_resource_with_finalizer(vm, combo, "QComboBox", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtComboBoxAddItemNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle combo = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!combo) return NULL_VAL;
    qt_combobox_add_item(combo, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtComboBoxGetCurrentTextNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle combo = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!combo) return NULL_VAL;
    return djazair_str(vm, qt_combobox_get_current_text(combo));
}

DJAZAIR_FUNC(qtComboBoxGetCurrentIndexNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle combo = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!combo) return NULL_VAL;
    return djazair_num(qt_combobox_get_current_index(combo));
}

DJAZAIR_FUNC(qtComboBoxSetCurrentIndexNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle combo = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!combo) return NULL_VAL;
    qt_combobox_set_current_index(combo, (int)djazair_get_num(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtProgressBarCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle bar = qt_progressbar_create(parent);
    return djazair_new_resource_with_finalizer(vm, bar, "QProgressBar", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtProgressBarSetRangeNative) {
    djazair_check_args(3, argCount);
    QtWidgetHandle bar = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!bar) return NULL_VAL;
    qt_progressbar_set_range(bar, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtProgressBarSetValueNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle bar = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!bar) return NULL_VAL;
    qt_progressbar_set_value(bar, (int)djazair_get_num(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtProgressBarGetValueNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle bar = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!bar) return NULL_VAL;
    return djazair_num(qt_progressbar_get_value(bar));
}

DJAZAIR_FUNC(qtSliderCreateNative) {
    int orient = (argCount > 0) ? (int)djazair_get_num(args, 0) : 0;
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle slider = qt_slider_create(orient, parent);
    return djazair_new_resource_with_finalizer(vm, slider, "QSlider", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtSliderSetRangeNative) {
    djazair_check_args(3, argCount);
    QtWidgetHandle slider = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!slider) return NULL_VAL;
    qt_slider_set_range(slider, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtSliderSetValueNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle slider = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!slider) return NULL_VAL;
    qt_slider_set_value(slider, (int)djazair_get_num(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtSliderGetValueNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle slider = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!slider) return NULL_VAL;
    return djazair_num(qt_slider_get_value(slider));
}

DJAZAIR_FUNC(qtSpinBoxCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle spin = qt_spinbox_create(parent);
    return djazair_new_resource_with_finalizer(vm, spin, "QSpinBox", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtSpinBoxSetRangeNative) {
    djazair_check_args(3, argCount);
    QtWidgetHandle spin = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!spin) return NULL_VAL;
    qt_spinbox_set_range(spin, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtSpinBoxGetValueNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle spin = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!spin) return NULL_VAL;
    return djazair_num(qt_spinbox_get_value(spin));
}

DJAZAIR_FUNC(qtSpinBoxSetValueNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle spin = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!spin) return NULL_VAL;
    qt_spinbox_set_value(spin, (int)djazair_get_num(args, 1));
    return djazair_null();
}

/* ============================================================
 * Containers (TabWidget, StackedWidget, GroupBox, ScrollArea, Splitter)
 * ============================================================ */
DJAZAIR_FUNC(qtTabWidgetCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle tabs = qt_tabwidget_create(parent);
    return djazair_new_resource_with_finalizer(vm, tabs, "QTabWidget", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtTabWidgetAddTabNative) {
    djazair_check_args(3, argCount);
    djazair_check_str(2);
    QtWidgetHandle tabs = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 1);
    if (!tabs || !w) return NULL_VAL;
    qt_tabwidget_add_tab(tabs, w, djazair_get_str(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtTabWidgetSetCurrentIndexNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle tabs = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!tabs) return NULL_VAL;
    qt_tabwidget_set_current_index(tabs, (int)djazair_get_num(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtStackedWidgetCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle stacked = qt_stackedwidget_create(parent);
    return djazair_new_resource_with_finalizer(vm, stacked, "QStackedWidget", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtStackedWidgetAddWidgetNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle stacked = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 1);
    if (!stacked || !w) return NULL_VAL;
    int idx = qt_stackedwidget_add_widget(stacked, w);
    return djazair_num(idx);
}

DJAZAIR_FUNC(qtStackedWidgetSetCurrentIndexNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle stacked = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!stacked) return NULL_VAL;
    qt_stackedwidget_set_current_index(stacked, (int)djazair_get_num(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtGroupBoxCreateNative) {
    const char* title = (argCount > 0 && djazair_is_string(args[0])) ? djazair_get_str(args, 0) : "";
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle group = qt_groupbox_create(title, parent);
    return djazair_new_resource_with_finalizer(vm, group, "QGroupBox", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtGroupBoxSetTitleNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle group = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!group) return NULL_VAL;
    qt_groupbox_set_title(group, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtScrollAreaCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle scroll = qt_scrollarea_create(parent);
    return djazair_new_resource_with_finalizer(vm, scroll, "QScrollArea", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtScrollAreaSetWidgetNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle scroll = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 1);
    if (!scroll || !w) return NULL_VAL;
    qt_scrollarea_set_widget(scroll, w);
    return djazair_null();
}

DJAZAIR_FUNC(qtSplitterCreateNative) {
    int orient = (argCount > 0) ? (int)djazair_get_num(args, 0) : 0;
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle splitter = qt_splitter_create(orient, parent);
    return djazair_new_resource_with_finalizer(vm, splitter, "QSplitter", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtSplitterAddWidgetNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle splitter = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 1);
    if (!splitter || !w) return NULL_VAL;
    qt_splitter_add_widget(splitter, w);
    return djazair_null();
}

/* ============================================================
 * Layout Managers & Spacing / Margins
 * ============================================================ */
DJAZAIR_FUNC(qtVBoxCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtLayoutHandle l = qt_vbox_create(parent);
    return djazair_new_resource_with_finalizer(vm, l, "QVBoxLayout", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtHBoxCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtLayoutHandle l = qt_hbox_create(parent);
    return djazair_new_resource_with_finalizer(vm, l, "QHBoxLayout", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtGridCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtLayoutHandle l = qt_grid_create(parent);
    return djazair_new_resource_with_finalizer(vm, l, "QGridLayout", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtFormCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtLayoutHandle l = qt_form_create(parent);
    return djazair_new_resource_with_finalizer(vm, l, "QFormLayout", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtLayoutAddWidgetNative) {
    djazair_check_args(2, argCount);
    QtLayoutHandle l = (QtLayoutHandle)get_qt_handle(vm, args, 0);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 1);
    if (!l || !w) return NULL_VAL;
    qt_layout_add_widget(l, w);
    return djazair_null();
}

DJAZAIR_FUNC(qtLayoutSetMarginsNative) {
    djazair_check_args(5, argCount);
    QtLayoutHandle l = (QtLayoutHandle)get_qt_handle(vm, args, 0);
    if (!l) return NULL_VAL;
    int left = (int)djazair_get_num(args, 1);
    int top = (int)djazair_get_num(args, 2);
    int right = (int)djazair_get_num(args, 3);
    int bottom = (int)djazair_get_num(args, 4);
    qt_layout_set_margins(l, left, top, right, bottom);
    return djazair_null();
}

DJAZAIR_FUNC(qtLayoutSetSpacingNative) {
    djazair_check_args(2, argCount);
    QtLayoutHandle l = (QtLayoutHandle)get_qt_handle(vm, args, 0);
    if (!l) return NULL_VAL;
    int spacing = (int)djazair_get_num(args, 1);
    qt_layout_set_spacing(l, spacing);
    return djazair_null();
}

DJAZAIR_FUNC(qtGridAddWidgetNative) {
    djazair_check_min_args(4, argCount);
    QtLayoutHandle l = (QtLayoutHandle)get_qt_handle(vm, args, 0);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 1);
    if (!l || !w) return NULL_VAL;
    int row = (int)djazair_get_num(args, 2);
    int col = (int)djazair_get_num(args, 3);
    int row_span = (argCount > 4) ? (int)djazair_get_num(args, 4) : 1;
    int col_span = (argCount > 5) ? (int)djazair_get_num(args, 5) : 1;
    qt_grid_add_widget(l, w, row, col, row_span, col_span);
    return djazair_null();
}

DJAZAIR_FUNC(qtFormAddRowNative) {
    djazair_check_args(3, argCount);
    djazair_check_str(1);
    QtLayoutHandle form = (QtLayoutHandle)get_qt_handle(vm, args, 0);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 2);
    if (!form || !w) return NULL_VAL;
    qt_form_add_row(form, djazair_get_str(args, 1), w);
    return djazair_null();
}

DJAZAIR_FUNC(qtLayoutAddLayoutNative) {
    djazair_check_args(2, argCount);
    QtLayoutHandle l = (QtLayoutHandle)get_qt_handle(vm, args, 0);
    QtLayoutHandle child = (QtLayoutHandle)get_qt_handle(vm, args, 1);
    if (!l || !child) return NULL_VAL;
    qt_layout_add_layout(l, child);
    return djazair_null();
}

/* ============================================================
 * Complex Views (List, Tree, Table)
 * ============================================================ */
DJAZAIR_FUNC(qtListWidgetCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle list = qt_listwidget_create(parent);
    return djazair_new_resource_with_finalizer(vm, list, "QListWidget", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtListWidgetAddItemNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle list = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!list) return NULL_VAL;
    qt_listwidget_add_item(list, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtListWidgetGetCurrentItemTextNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle list = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!list) return NULL_VAL;
    return djazair_str(vm, qt_listwidget_get_current_item_text(list));
}

DJAZAIR_FUNC(qtListWidgetClearNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle list = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!list) return NULL_VAL;
    qt_listwidget_clear(list);
    return djazair_null();
}

DJAZAIR_FUNC(qtTreeWidgetCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle tree = qt_treewidget_create(parent);
    return djazair_new_resource_with_finalizer(vm, tree, "QTreeWidget", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtTreeWidgetSetHeadersNative) {
    djazair_check_args(3, argCount);
    QtWidgetHandle tree = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    djazair_value arrVal = args[1];
    if (!IS_ARRAY(arrVal)) return NULL_VAL;
    int count = djazair_array_len(vm, arrVal);
    const char** cols = (const char**)malloc(sizeof(char*) * count);
    for (int i = 0; i < count; i++) {
        djazair_value item = djazair_array_get(vm, arrVal, i);
        cols[i] = IS_STRING(item) ? AS_CSTRING(item) : "";
    }
    qt_treewidget_set_headers(tree, cols, count);
    free(cols);
    return djazair_null();
}

DJAZAIR_FUNC(qtTreeWidgetAddItemNative) {
    djazair_check_min_args(2, argCount);
    QtWidgetHandle tree = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    void* parent_item = (argCount > 2 && djazair_is_resource(args[2])) ? djazair_get_resource(vm, args[2]) : NULL;
    
    djazair_value arrVal = args[1];
    if (!IS_ARRAY(arrVal)) return NULL_VAL;
    int count = djazair_array_len(vm, arrVal);
    const char** cols = (const char**)malloc(sizeof(char*) * count);
    for (int i = 0; i < count; i++) {
        djazair_value item = djazair_array_get(vm, arrVal, i);
        cols[i] = IS_STRING(item) ? AS_CSTRING(item) : "";
    }
    void* itemPtr = qt_treewidget_add_item(tree, parent_item, cols, count);
    free(cols);
    return djazair_new_resource(vm, itemPtr, "QTreeWidgetItem");
}

DJAZAIR_FUNC(qtTableWidgetCreateNative) {
    int rows = (argCount > 0) ? (int)djazair_get_num(args, 0) : 0;
    int cols = (argCount > 1) ? (int)djazair_get_num(args, 1) : 0;
    QtWidgetHandle parent = (argCount > 2 && djazair_is_resource(args[2])) ? (QtWidgetHandle)djazair_get_resource(vm, args[2]) : NULL;
    QtWidgetHandle table = qt_tablewidget_create(rows, cols, parent);
    return djazair_new_resource_with_finalizer(vm, table, "QTableWidget", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtTableWidgetSetItemNative) {
    djazair_check_args(4, argCount);
    QtWidgetHandle table = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!table) return NULL_VAL;
    int row = (int)djazair_get_num(args, 1);
    int col = (int)djazair_get_num(args, 2);
    qt_tablewidget_set_item(table, row, col, djazair_get_str(args, 3));
    return djazair_null();
}

DJAZAIR_FUNC(qtTableWidgetSetSizeNative) {
    djazair_check_args(3, argCount);
    QtWidgetHandle table = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!table) return NULL_VAL;
    qt_tablewidget_set_size(table, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtTableWidgetSetHeadersNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle table = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!table) return NULL_VAL;
    djazair_value arrVal = args[1];
    if (!IS_ARRAY(arrVal)) return NULL_VAL;
    int count = djazair_array_len(vm, arrVal);
    const char** cols = (const char**)malloc(sizeof(char*) * count);
    for (int i = 0; i < count; i++) {
        djazair_value item = djazair_array_get(vm, arrVal, i);
        cols[i] = IS_STRING(item) ? AS_CSTRING(item) : "";
    }
    qt_tablewidget_set_headers(table, cols, count);
    free(cols);
    return djazair_null();
}

DJAZAIR_FUNC(qtTableWidgetGetItemNative) {
    djazair_check_args(3, argCount);
    QtWidgetHandle table = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!table) return NULL_VAL;
    int row = (int)djazair_get_num(args, 1);
    int col = (int)djazair_get_num(args, 2);
    return djazair_str(vm, qt_tablewidget_get_item(table, row, col));
}

/* ============================================================
 * System Tray & Context Menus
 * ============================================================ */
DJAZAIR_FUNC(qtTrayCreateNative) {
    const char* tooltip = (argCount > 0 && djazair_is_string(args[0])) ? djazair_get_str(args, 0) : "Djazair App";
    QtTrayHandle tray = qt_tray_create(tooltip);
    return djazair_new_resource_with_finalizer(vm, tray, "QSystemTrayIcon", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtTrayShowNative) {
    djazair_check_args(1, argCount);
    QtTrayHandle tray = (QtTrayHandle)get_qt_handle(vm, args, 0);
    if (!tray) return NULL_VAL;
    qt_tray_show(tray);
    return djazair_null();
}

DJAZAIR_FUNC(qtTraySetIconNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtTrayHandle tray = (QtTrayHandle)get_qt_handle(vm, args, 0);
    if (!tray) return NULL_VAL;
    qt_tray_set_icon(tray, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtTraySetMenuNative) {
    djazair_check_args(2, argCount);
    QtTrayHandle tray = (QtTrayHandle)get_qt_handle(vm, args, 0);
    QtMenuHandle menu = (QtMenuHandle)get_qt_handle(vm, args, 1);
    if (!tray || !menu) return NULL_VAL;
    qt_tray_set_menu(tray, menu);
    return djazair_null();
}

DJAZAIR_FUNC(qtTrayShowMessageNative) {
    djazair_check_args(5, argCount);
    QtTrayHandle tray = (QtTrayHandle)get_qt_handle(vm, args, 0);
    if (!tray) return NULL_VAL;
    const char* title = djazair_get_str(args, 1);
    const char* msg = djazair_get_str(args, 2);
    int icon_type = (int)djazair_get_num(args, 3);
    int timeout = (int)djazair_get_num(args, 4);
    qt_tray_show_message(tray, title, msg, icon_type, timeout);
    return djazair_null();
}

DJAZAIR_FUNC(qtTrayHideNative) {
    djazair_check_args(1, argCount);
    QtTrayHandle tray = (QtTrayHandle)get_qt_handle(vm, args, 0);
    if (!tray) return NULL_VAL;
    qt_tray_hide(tray);
    return djazair_null();
}

/* ============================================================
 * Dialogs & All MessageBox Variants
 * ============================================================ */
DJAZAIR_FUNC(qtMessageBoxInfoNative) {
    djazair_check_args(2, argCount);
    qt_messagebox_info(NULL, djazair_get_str(args, 0), djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtMessageBoxWarningNative) {
    djazair_check_args(2, argCount);
    qt_messagebox_warning(NULL, djazair_get_str(args, 0), djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtMessageBoxErrorNative) {
    djazair_check_args(2, argCount);
    qt_messagebox_error(NULL, djazair_get_str(args, 0), djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtMessageBoxQuestionNative) {
    djazair_check_args(2, argCount);
    bool res = qt_messagebox_question(NULL, djazair_get_str(args, 0), djazair_get_str(args, 1));
    return djazair_bool(res);
}

DJAZAIR_FUNC(qtFileDialogOpenNative) {
    const char* title  = (argCount > 0) ? djazair_get_str(args, 0) : "Open File";
    const char* filter = (argCount > 1) ? djazair_get_str(args, 1) : "All Files (*.*)";
    return djazair_str(vm, qt_filedialog_get_open_file_name(NULL, title, filter));
}

DJAZAIR_FUNC(qtFileDialogSaveNative) {
    const char* title  = (argCount > 0) ? djazair_get_str(args, 0) : "Save File";
    const char* filter = (argCount > 1) ? djazair_get_str(args, 1) : "All Files (*.*)";
    return djazair_str(vm, qt_filedialog_get_save_file_name(NULL, title, filter));
}

DJAZAIR_FUNC(qtFileDialogGetDirectoryNative) {
    const char* title  = (argCount > 0) ? djazair_get_str(args, 0) : "Select Directory";
    return djazair_str(vm, qt_filedialog_get_existing_directory(NULL, title));
}

DJAZAIR_FUNC(qtInputDialogGetTextNative) {
    djazair_check_args(2, argCount);
    const char* title = djazair_get_str(args, 0);
    const char* label = djazair_get_str(args, 1);
    return djazair_str(vm, qt_inputdialog_get_text(NULL, title, label));
}

/* ============================================================
 * Canvas & Drawing (QPixmap & QPainter)
 * ============================================================ */
DJAZAIR_FUNC(qtPixmapCreateNative) {
    djazair_check_args(2, argCount);
    int w = (int)djazair_get_num(args, 0);
    int h = (int)djazair_get_num(args, 1);
    QtPixmapHandle pix = qt_pixmap_create(w, h);
    return djazair_new_resource(vm, pix, "QPixmap");
}

DJAZAIR_FUNC(qtPixmapFillNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtPixmapHandle pix = (QtPixmapHandle)get_qt_handle(vm, args, 0);
    if (!pix) return NULL_VAL;
    qt_pixmap_fill(pix, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtPixmapDestroyNative) {
    djazair_check_args(1, argCount);
    QtPixmapHandle pix = (QtPixmapHandle)get_qt_handle(vm, args, 0);
    if (!pix) return NULL_VAL;
    qt_pixmap_destroy(pix);
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterCreateNative) {
    djazair_check_args(1, argCount);
    QtPixmapHandle pix = (QtPixmapHandle)get_qt_handle(vm, args, 0);
    if (!pix) return NULL_VAL;
    QtPainterHandle painter = qt_painter_create(pix);
    return djazair_new_resource(vm, painter, "QPainter");
}

DJAZAIR_FUNC(qtPainterSetPenNative) {
    djazair_check_args(3, argCount);
    djazair_check_str(1);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_set_pen(p, djazair_get_str(args, 1), (int)djazair_get_num(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterSetBrushNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_set_brush(p, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterDrawLineNative) {
    djazair_check_args(5, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_draw_line(p, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2), (int)djazair_get_num(args, 3), (int)djazair_get_num(args, 4));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterDrawRectNative) {
    djazair_check_args(5, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_draw_rect(p, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2), (int)djazair_get_num(args, 3), (int)djazair_get_num(args, 4));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterDrawEllipseNative) {
    djazair_check_args(5, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_draw_ellipse(p, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2), (int)djazair_get_num(args, 3), (int)djazair_get_num(args, 4));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterDrawTextNative) {
    djazair_check_args(4, argCount);
    djazair_check_str(3);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_draw_text(p, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2), djazair_get_str(args, 3));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterEndNative) {
    djazair_check_args(1, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_end(p);
    return djazair_null();
}

/* ============================================================
 * Qt Charts Native Bindings
 * ============================================================ */
DJAZAIR_FUNC(qtChartViewCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtChartViewHandle view = qt_chartview_create(parent);
    return djazair_new_resource_with_finalizer(vm, view, "QChartView", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtChartViewSetChartNative) {
    djazair_check_args(2, argCount);
    QtChartViewHandle view = (QtChartViewHandle)get_qt_handle(vm, args, 0);
    QtChartHandle chart = (QtChartHandle)get_qt_handle(vm, args, 1);
    if (!view || !chart) return NULL_VAL;
    qt_chartview_set_chart(view, chart);
    return djazair_null();
}

DJAZAIR_FUNC(qtChartCreateNative) {
    QtChartHandle chart = qt_chart_create();
    return djazair_new_resource_with_finalizer(vm, chart, "QChart", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtChartSetTitleNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtChartHandle chart = (QtChartHandle)get_qt_handle(vm, args, 0);
    if (!chart) return NULL_VAL;
    qt_chart_set_title(chart, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtChartAddSeriesNative) {
    djazair_check_args(2, argCount);
    QtChartHandle chart = (QtChartHandle)get_qt_handle(vm, args, 0);
    QtSeriesHandle series = (QtSeriesHandle)get_qt_handle(vm, args, 1);
    if (!chart || !series) return NULL_VAL;
    qt_chart_add_series(chart, series);
    return djazair_null();
}

DJAZAIR_FUNC(qtChartCreateDefaultAxesNative) {
    djazair_check_args(1, argCount);
    QtChartHandle chart = (QtChartHandle)get_qt_handle(vm, args, 0);
    if (!chart) return NULL_VAL;
    qt_chart_create_default_axes(chart);
    return djazair_null();
}

DJAZAIR_FUNC(qtLineSeriesCreateNative) {
    QtSeriesHandle series = qt_lineseries_create();
    return djazair_new_resource_with_finalizer(vm, series, "QLineSeries", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtLineSeriesAppendNative) {
    djazair_check_args(3, argCount);
    QtSeriesHandle series = (QtSeriesHandle)get_qt_handle(vm, args, 0);
    if (!series) return NULL_VAL;
    qt_lineseries_append(series, djazair_get_num(args, 1), djazair_get_num(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtPieSeriesCreateNative) {
    QtSeriesHandle series = qt_pieseries_create();
    return djazair_new_resource_with_finalizer(vm, series, "QPieSeries", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtPieSeriesAppendNative) {
    djazair_check_args(3, argCount);
    djazair_check_str(1);
    QtSeriesHandle series = (QtSeriesHandle)get_qt_handle(vm, args, 0);
    if (!series) return NULL_VAL;
    qt_pieseries_append(series, djazair_get_str(args, 1), djazair_get_num(args, 2));
    return djazair_null();
}

/* ============================================================
 * Drag & Drop
 * ============================================================ */
DJAZAIR_FUNC(qtWidgetSetAcceptDropsNative) {
    djazair_check_args(2, argCount);
    djazair_check_bool(1);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    qt_widget_set_accept_drops(w, djazair_get_bool(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtTimerCreateNative) {
    QtTimerHandle timer = qt_timer_create();
    return djazair_new_resource_with_finalizer(vm, timer, "QTimer", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtTimerStartNative) {
    djazair_check_args(2, argCount);
    QtTimerHandle timer = (QtTimerHandle)get_qt_handle(vm, args, 0);
    if (!timer) return NULL_VAL;
    int msec = (int)djazair_get_num(args, 1);
    qt_timer_start(timer, msec);
    return djazair_null();
}

DJAZAIR_FUNC(qtTimerStopNative) {
    djazair_check_args(1, argCount);
    QtTimerHandle timer = (QtTimerHandle)get_qt_handle(vm, args, 0);
    if (!timer) return NULL_VAL;
    qt_timer_stop(timer);
    return djazair_null();
}

/* ============================================================
 * Signal/Slot Callback System
 * ============================================================ */

/* Global VM pointer required for callbacks from Qt signal handlers */
static djazairVM* g_qt_vm = NULL;

/* Build a unique map key from widget handle + signal name */
static std::string callbackKey(void* handle, const char* signal) {
    char buf[128];
    snprintf(buf, sizeof(buf), "%p|%s", handle, signal);
    return std::string(buf);
}

/* Get or create the hidden callback registry map in VM globals */
static djazair_value getCallbackMap(djazairVM* vm) {
    djazair_value key = djazair_str(vm, "__qt_callbacks");
    djazair_value result;
    if (tableGet(vm, &vm->globals, key, &result)) {
        return result;
    }
    result = djazair_new_map(vm);
    tableSet(vm, &vm->globals, key, result);
    return result;
}

/* Store a Djazair closure so GC will not collect it */
static void setCallback(void* handle, const char* signal, djazair_value closure) {
    djazair_value map = getCallbackMap(g_qt_vm);
    std::string keyStr = callbackKey(handle, signal);
    djazair_value key = djazair_str(g_qt_vm, keyStr.c_str());
    djazair_map_set(g_qt_vm, map, key, closure);
}

/* Look up a previously stored closure from the registry */
static djazair_value getCallback(void* handle, const char* signal) {
    djazair_value map = getCallbackMap(g_qt_vm);
    std::string keyStr = callbackKey(handle, signal);
    djazair_value key = djazair_str(g_qt_vm, keyStr.c_str());
    djazair_value result;
    if (djazair_map_get(g_qt_vm, map, key, &result)) {
        return result;
    }
    return djazair_null();
}

/* Invoke a stored callback with proper GC protection */
static void invokeVoidCallback(void* handle, const char* signal) {
    djazair_value closure = getCallback(handle, signal);
    if (IS_NULL(closure)) return;
    djazair_push(g_qt_vm, closure);
    callValue(g_qt_vm, closure, 0);
    djazair_pop(g_qt_vm);  // remove return value
}

static void invokeIntCallback(void* handle, const char* signal, int value) {
    djazair_value closure = getCallback(handle, signal);
    if (IS_NULL(closure)) return;
    djazair_push(g_qt_vm, closure);
    djazair_push(g_qt_vm, djazair_num(value));
    callValue(g_qt_vm, closure, 1);
    djazair_pop(g_qt_vm);
}

static void invokeStringCallback(void* handle, const char* signal, const char* text) {
    djazair_value closure = getCallback(handle, signal);
    if (IS_NULL(closure)) return;
    djazair_push(g_qt_vm, closure);
    djazair_push(g_qt_vm, djazair_str(g_qt_vm, text));
    callValue(g_qt_vm, closure, 1);
    djazair_pop(g_qt_vm);
}

/* Qt signal trampoline callbacks */

static void qtVoidTrampoline(void* user_data) {
    void* handle = user_data;
    invokeVoidCallback(handle, "clicked");
}

static void qtIntTrampoline(int value, void* user_data) {
    void* handle = user_data;
    invokeIntCallback(handle, "changed", value);
}

static void qtStringTrampoline(const char* text, void* user_data) {
    void* handle = user_data;
    invokeStringCallback(handle, "changed", text);
}

static void qtTextEditTrampoline(void* user_data) {
    void* handle = user_data;
    invokeVoidCallback(handle, "changed");
}

static void qtTimerTrampoline(void* user_data) {
    void* handle = user_data;
    invokeVoidCallback(handle, "timeout");
}

/* ============================================================
 * Signal Binding Native Functions
 * ============================================================ */

DJAZAIR_FUNC(qtButtonOnClickedNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle btn = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!btn) return NULL_VAL;
    if (!IS_CLOSURE(args[1]) && !IS_FUNCTION(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be a function/closure.");
        return NULL_VAL;
    }
    setCallback(btn, "clicked", args[1]);
    qt_button_on_click(btn, qtVoidTrampoline, btn);
    return djazair_null();
}

DJAZAIR_FUNC(qtCheckBoxOnToggledNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle cb = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!cb) return NULL_VAL;
    if (!IS_CLOSURE(args[1]) && !IS_FUNCTION(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be a function/closure.");
        return NULL_VAL;
    }
    setCallback(cb, "changed", args[1]);
    qt_checkbox_on_toggled(cb, qtIntTrampoline, cb);
    return djazair_null();
}

DJAZAIR_FUNC(qtComboBoxOnChangeNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle combo = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!combo) return NULL_VAL;
    if (!IS_CLOSURE(args[1]) && !IS_FUNCTION(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be a function/closure.");
        return NULL_VAL;
    }
    setCallback(combo, "changed", args[1]);
    qt_combobox_on_change(combo, qtIntTrampoline, combo);
    return djazair_null();
}

DJAZAIR_FUNC(qtLineEditOnChangeNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    if (!IS_CLOSURE(args[1]) && !IS_FUNCTION(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be a function/closure.");
        return NULL_VAL;
    }
    setCallback(edit, "changed", args[1]);
    qt_lineedit_on_change(edit, qtStringTrampoline, edit);
    return djazair_null();
}

DJAZAIR_FUNC(qtTextEditOnChangeNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    if (!IS_CLOSURE(args[1]) && !IS_FUNCTION(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be a function/closure.");
        return NULL_VAL;
    }
    setCallback(edit, "changed", args[1]);
    qt_textedit_on_change(edit, qtTextEditTrampoline, edit);
    return djazair_null();
}

DJAZAIR_FUNC(qtSliderOnValueChangedNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle slider = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!slider) return NULL_VAL;
    if (!IS_CLOSURE(args[1]) && !IS_FUNCTION(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be a function/closure.");
        return NULL_VAL;
    }
    setCallback(slider, "changed", args[1]);
    qt_slider_on_value_changed(slider, qtIntTrampoline, slider);
    return djazair_null();
}

DJAZAIR_FUNC(qtSpinBoxOnChangeNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle spin = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!spin) return NULL_VAL;
    if (!IS_CLOSURE(args[1]) && !IS_FUNCTION(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be a function/closure.");
        return NULL_VAL;
    }
    setCallback(spin, "changed", args[1]);
    qt_spinbox_on_change(spin, qtIntTrampoline, spin);
    return djazair_null();
}

DJAZAIR_FUNC(qtTimerOnTimeoutNative) {
    djazair_check_args(2, argCount);
    QtTimerHandle timer = (QtTimerHandle)get_qt_handle(vm, args, 0);
    if (!timer) return NULL_VAL;
    if (!IS_CLOSURE(args[1]) && !IS_FUNCTION(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be a function/closure.");
        return NULL_VAL;
    }
    setCallback(timer, "timeout", args[1]);
    qt_timer_on_timeout(timer, qtTimerTrampoline, timer);
    return djazair_null();
}

DJAZAIR_FUNC(qtWidgetOnDropNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle w = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!w) return NULL_VAL;
    if (!IS_CLOSURE(args[1]) && !IS_FUNCTION(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be a function/closure.");
        return NULL_VAL;
    }
    setCallback(w, "drop", args[1]);
    qt_widget_on_drop(w, qtStringTrampoline, w);
    return djazair_null();
}

/* ============================================================
 * Pixmap Save/Load & Extended Drawing Native Bindings
 * ============================================================ */
DJAZAIR_FUNC(qtPixmapSaveNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtPixmapHandle pix = (QtPixmapHandle)get_qt_handle(vm, args, 0);
    if (!pix) return NULL_VAL;
    bool ok = qt_pixmap_save(pix, djazair_get_str(args, 1));
    return ok ? djazair_true() : djazair_false();
}

DJAZAIR_FUNC(qtPixmapLoadNative) {
    djazair_check_args(1, argCount);
    djazair_check_str(0);
    QtPixmapHandle pix = qt_pixmap_load(djazair_get_str(args, 0));
    if (!pix) return NULL_VAL;
    return djazair_new_resource_with_finalizer(vm, pix, "QPixmap", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtPainterDrawRoundedRectNative) {
    djazair_check_args(6, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_draw_rounded_rect(p, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2),
                                 (int)djazair_get_num(args, 3), (int)djazair_get_num(args, 4),
                                 (int)djazair_get_num(args, 5));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterDrawArcNative) {
    djazair_check_args(7, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_draw_arc(p, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2),
                        (int)djazair_get_num(args, 3), (int)djazair_get_num(args, 4),
                        (int)djazair_get_num(args, 5), (int)djazair_get_num(args, 6));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterDrawChordNative) {
    djazair_check_args(7, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_draw_chord(p, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2),
                          (int)djazair_get_num(args, 3), (int)djazair_get_num(args, 4),
                          (int)djazair_get_num(args, 5), (int)djazair_get_num(args, 6));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterDrawPieNative) {
    djazair_check_args(7, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_draw_pie(p, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2),
                        (int)djazair_get_num(args, 3), (int)djazair_get_num(args, 4),
                        (int)djazair_get_num(args, 5), (int)djazair_get_num(args, 6));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterDrawPolygonNative) {
    djazair_check_args(2, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    if (!djazair_is_array(args[1])) {
        runtimeError(vm, "TypeError: Argument 2 must be an array of [x,y,...]");
        return NULL_VAL;
    }
    djazair_value arr = args[1];
    int count = djazair_array_len(vm, arr) / 2;
    if (count < 3) {
        runtimeError(vm, "TypeError: Need at least 3 points for polygon");
        return NULL_VAL;
    }
    int* points = (int*)malloc(count * 2 * sizeof(int));
    for (int i = 0; i < count * 2; ++i) {
        points[i] = (int)AS_NUMBER(djazair_array_get(vm, arr, i));
    }
    qt_painter_draw_polygon(p, points, count);
    free(points);
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterDrawPixmapNative) {
    djazair_check_args(4, argCount);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    QtPixmapHandle pix = (QtPixmapHandle)get_qt_handle(vm, args, 1);
    if (!p || !pix) return NULL_VAL;
    qt_painter_draw_pixmap(p, pix, (int)djazair_get_num(args, 2), (int)djazair_get_num(args, 3));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterSetPenStyleNative) {
    djazair_check_args(4, argCount);
    djazair_check_str(3);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_set_pen_style(p, (int)djazair_get_num(args, 1),
                             (int)djazair_get_num(args, 2),
                             djazair_get_str(args, 3));
    return djazair_null();
}

DJAZAIR_FUNC(qtPainterSetFontNative) {
    djazair_check_args(5, argCount);
    djazair_check_str(1);
    QtPainterHandle p = (QtPainterHandle)get_qt_handle(vm, args, 0);
    if (!p) return NULL_VAL;
    qt_painter_set_font(p, djazair_get_str(args, 1),
                        (int)djazair_get_num(args, 2),
                        djazair_get_bool(args, 3),
                        djazair_get_bool(args, 4));
    return djazair_null();
}

/* ============================================================
 * Canvas Widget Native Bindings
 * ============================================================ */
DJAZAIR_FUNC(qtCanvasCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle canvas = qt_canvas_create(parent);
    return djazair_new_resource_with_finalizer(vm, canvas, "CanvasWidget", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtCanvasBeginNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle canvas = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!canvas) return NULL_VAL;
    QtPainterHandle p = qt_canvas_begin(canvas);
    if (!p) return NULL_VAL;
    return djazair_new_resource(vm, p, "QPainter");
}

DJAZAIR_FUNC(qtCanvasEndNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle canvas = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!canvas) return NULL_VAL;
    qt_canvas_end(canvas);
    return djazair_null();
}

DJAZAIR_FUNC(qtCanvasClearNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle canvas = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!canvas) return NULL_VAL;
    qt_canvas_clear(canvas, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtCanvasSetSizeNative) {
    djazair_check_args(3, argCount);
    QtWidgetHandle canvas = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!canvas) return NULL_VAL;
    qt_canvas_set_size(canvas, (int)djazair_get_num(args, 1), (int)djazair_get_num(args, 2));
    return djazair_null();
}

/* ============================================================
 * Extended Charts Native Bindings (Bar, Scatter, Axes)
 * ============================================================ */
DJAZAIR_FUNC(qtBarSeriesCreateNative) {
    QtSeriesHandle series = qt_barseries_create();
    return djazair_new_resource_with_finalizer(vm, series, "QBarSeries", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtBarSetCreateNative) {
    djazair_check_args(1, argCount);
    djazair_check_str(0);
    QtSeriesHandle barset = qt_barset_create(djazair_get_str(args, 0));
    return djazair_new_resource_with_finalizer(vm, barset, "QBarSet", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtBarSetAppendNative) {
    djazair_check_args(2, argCount);
    QtSeriesHandle barset = (QtSeriesHandle)get_qt_handle(vm, args, 0);
    if (!barset) return NULL_VAL;
    qt_barset_append(barset, djazair_get_num(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtBarSeriesAppendNative) {
    djazair_check_args(2, argCount);
    QtSeriesHandle series = (QtSeriesHandle)get_qt_handle(vm, args, 0);
    QtSeriesHandle barset = (QtSeriesHandle)get_qt_handle(vm, args, 1);
    if (!series || !barset) return NULL_VAL;
    qt_barseries_append(series, barset);
    return djazair_null();
}

DJAZAIR_FUNC(qtScatterSeriesCreateNative) {
    QtSeriesHandle series = qt_scatterseries_create();
    return djazair_new_resource_with_finalizer(vm, series, "QScatterSeries", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtScatterSeriesAppendNative) {
    djazair_check_args(3, argCount);
    QtSeriesHandle series = (QtSeriesHandle)get_qt_handle(vm, args, 0);
    if (!series) return NULL_VAL;
    qt_scatterseries_append(series, djazair_get_num(args, 1), djazair_get_num(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtChartSetAxisTitleNative) {
    djazair_check_args(3, argCount);
    djazair_check_str(2);
    QtChartHandle chart = (QtChartHandle)get_qt_handle(vm, args, 0);
    if (!chart) return NULL_VAL;
    qt_chart_set_axis_title(chart, (int)djazair_get_num(args, 1), djazair_get_str(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtChartSetAxisRangeNative) {
    djazair_check_args(4, argCount);
    QtChartHandle chart = (QtChartHandle)get_qt_handle(vm, args, 0);
    if (!chart) return NULL_VAL;
    qt_chart_set_axis_range(chart, (int)djazair_get_num(args, 1), djazair_get_num(args, 2), djazair_get_num(args, 3));
    return djazair_null();
}

/* ============================================================
 * QDialog Native Bindings
 * ============================================================ */
DJAZAIR_FUNC(qtDialogCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle dlg = qt_dialog_create(parent);
    return djazair_new_resource_with_finalizer(vm, dlg, "QDialog", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtDialogExecNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle dlg = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!dlg) return NULL_VAL;
    int result = qt_dialog_exec(dlg);
    return djazair_num(result);
}

/* ============================================================
 * QDoubleSpinBox Native Bindings
 * ============================================================ */
DJAZAIR_FUNC(qtDoubleSpinBoxCreateNative) {
    QtWidgetHandle parent = (argCount > 0 && djazair_is_resource(args[0])) ? (QtWidgetHandle)djazair_get_resource(vm, args[0]) : NULL;
    QtWidgetHandle spin = qt_doublespinbox_create(parent);
    return djazair_new_resource_with_finalizer(vm, spin, "QDoubleSpinBox", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtDoubleSpinBoxSetRangeNative) {
    djazair_check_args(3, argCount);
    QtWidgetHandle spin = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!spin) return NULL_VAL;
    qt_doublespinbox_set_range(spin, djazair_get_num(args, 1), djazair_get_num(args, 2));
    return djazair_null();
}

DJAZAIR_FUNC(qtDoubleSpinBoxSetValueNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle spin = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!spin) return NULL_VAL;
    qt_doublespinbox_set_value(spin, djazair_get_num(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtDoubleSpinBoxGetValueNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle spin = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!spin) return NULL_VAL;
    return djazair_num(qt_doublespinbox_get_value(spin));
}

DJAZAIR_FUNC(qtDoubleSpinBoxSetDecimalsNative) {
    djazair_check_args(2, argCount);
    QtWidgetHandle spin = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!spin) return NULL_VAL;
    qt_doublespinbox_set_decimals(spin, (int)djazair_get_num(args, 1));
    return djazair_null();
}

/* ============================================================
 * QPlainTextEdit Native Bindings
 * ============================================================ */
DJAZAIR_FUNC(qtPlainTextEditCreateNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(0);
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle edit = qt_plaintextedit_create(djazair_get_str(args, 0), parent);
    return djazair_new_resource_with_finalizer(vm, edit, "QPlainTextEdit", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtPlainTextEditAppendNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    qt_plaintextedit_append(edit, djazair_get_str(args, 1));
    return djazair_null();
}

DJAZAIR_FUNC(qtPlainTextEditClearNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    qt_plaintextedit_clear(edit);
    return djazair_null();
}

DJAZAIR_FUNC(qtPlainTextEditGetTextNative) {
    djazair_check_args(1, argCount);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    return djazair_str(vm, qt_plaintextedit_get_text(edit));
}

DJAZAIR_FUNC(qtPlainTextEditSetReadOnlyNative) {
    djazair_check_args(2, argCount);
    djazair_check_bool(1);
    QtWidgetHandle edit = (QtWidgetHandle)get_qt_handle(vm, args, 0);
    if (!edit) return NULL_VAL;
    qt_plaintextedit_set_read_only(edit, djazair_get_bool(args, 1));
    return djazair_null();
}

/* ============================================================
 * Dispatch Method Table
 * ============================================================ */
static NativeMethod qt_module_funcs[] = {
    {"appCreate",             qtAppCreateNative,             -1},
    {"appExec",               qtAppExecNative,               1},
    {"appProcessEvents",      qtAppProcessEventsNative,      1},
    {"appQuit",               qtAppQuitNative,               1},
    {"appSetStyle",           qtAppSetStyleNative,           1},
    {"appDestroy",            qtAppDestroyNative,            1},

    {"loadUi",                qtLoadUiNative,                -1},

    {"mediaplayerCreate",     qtMediaPlayerCreateNative,     0},
    {"mediaplayerSetMedia",   qtMediaPlayerSetMediaNative,   2},
    {"mediaplayerPlay",       qtMediaPlayerPlayNative,       1},
    {"mediaplayerPause",      qtMediaPlayerPauseNative,      1},
    {"mediaplayerStop",       qtMediaPlayerStopNative,       1},
    {"videowidgetCreate",     qtVideoWidgetCreateNative,     -1},
    {"mediaplayerSetVideoOutput", qtMediaPlayerSetVideoOutputNative, 2},

    {"widgetCreate",          qtWidgetCreateNative,          -1},
    {"widgetShow",            qtWidgetShowNative,            1},
    {"widgetHide",            qtWidgetHideNative,            1},
    {"widgetSetTitle",        qtWidgetSetTitleNative,        2},
    {"widgetSetGeometry",     qtWidgetSetGeometryNative,     5},
    {"widgetResize",          qtWidgetResizeNative,          3},
    {"widgetSetLayout",       qtWidgetSetLayoutNative,       2},
    {"widgetSetStyleSheet",   qtWidgetSetStyleSheetNative,   2},
    {"widgetSetEnabled",      qtWidgetSetEnabledNative,      2},
    {"widgetSetToolTip",      qtWidgetSetToolTipNative,      2},
    {"widgetUpdate",          qtWidgetUpdateNative,          1},
    {"widgetDestroy",         qtWidgetDestroyNative,         1},

    {"mainwindowCreate",      qtMainWindowCreateNative,      0},
    {"mainwindowSetCentral",  qtMainWindowSetCentralWidgetNative, 2},
    {"mainwindowSetTitle",    qtMainWindowSetTitleNative,    2},
    {"mainwindowResize",      qtMainWindowResizeNative,      3},
    {"mainwindowShow",        qtMainWindowShowNative,        1},
    {"mainwindowAddMenu",     qtMainWindowAddMenuNative,     2},
    {"mainwindowAddToolBar",  qtMainWindowAddToolBarNative,  2},
    {"toolbarAddAction",      qtToolBarAddActionNative,      2},
    {"mainwindowSetStatus",   qtMainWindowSetStatusMessageNative, 3},

    {"menuAddSubmenu",        qtMenuAddSubmenuNative,        2},
    {"menuAddAction",         qtMenuAddActionNative,         -1},
    {"menuAddSeparator",      qtMenuAddSeparatorNative,      1},

    {"buttonCreate",          qtButtonCreateNative,          -1},
    {"buttonSetText",         qtButtonSetTextNative,         2},
    {"buttonOnClicked",       qtButtonOnClickedNative,       2},

    {"labelCreate",           qtLabelCreateNative,           -1},
    {"labelSetText",          qtLabelSetTextNative,          2},
    {"labelGetText",          qtLabelGetTextNative,          1},
    {"labelSetAlignment",     qtLabelSetAlignmentNative,     2},
    {"labelSetPixmap",        qtLabelSetPixmapNative,        2},

    {"lineeditCreate",        qtLineEditCreateNative,        -1},
    {"lineeditSetText",       qtLineEditSetTextNative,       2},
    {"lineeditGetText",       qtLineEditGetTextNative,       1},
    {"lineeditSetPlaceholder",qtLineEditSetPlaceholderNative,2},
    {"lineeditSetEchoMode",   qtLineEditSetEchoModeNative,   2},
    {"lineeditOnChange",      qtLineEditOnChangeNative,      2},

    {"texteditCreate",        qtTextEditCreateNative,        -1},
    {"texteditSetText",       qtTextEditSetTextNative,       2},
    {"texteditGetText",       qtTextEditGetTextNative,       1},
    {"texteditClear",         qtTextEditClearNative,         1},
    {"texteditAppend",        qtTextEditAppendNative,        2},
    {"texteditOnChange",      qtTextEditOnChangeNative,      2},

    {"checkboxCreate",        qtCheckBoxCreateNative,        -1},
    {"checkboxIsChecked",     qtCheckBoxIsCheckedNative,     1},
    {"checkboxSetChecked",    qtCheckBoxSetCheckedNative,    2},
    {"checkboxOnToggled",     qtCheckBoxOnToggledNative,     2},
    {"radiobuttonCreate",     qtRadioButtonCreateNative,     -1},
    {"radiobuttonSetChecked", qtRadioButtonSetCheckedNative, 2},
    {"radiobuttonIsChecked",  qtRadioButtonIsCheckedNative,  1},
    {"comboboxCreate",        qtComboBoxCreateNative,        -1},
    {"comboboxAddItem",       qtComboBoxAddItemNative,       2},
    {"comboboxGetCurrent",    qtComboBoxGetCurrentTextNative, 1},
    {"comboboxGetCurrentIndex",qtComboBoxGetCurrentIndexNative,1},
    {"comboboxSetCurrentIndex",qtComboBoxSetCurrentIndexNative,2},
    {"comboboxOnChange",      qtComboBoxOnChangeNative,      2},
    {"progressbarCreate",     qtProgressBarCreateNative,     -1},
    {"progressbarSetRange",   qtProgressBarSetRangeNative,   3},
    {"progressbarSetValue",   qtProgressBarSetValueNative,   2},
    {"progressbarGetValue",   qtProgressBarGetValueNative,   1},
    {"sliderCreate",          qtSliderCreateNative,          -1},
    {"sliderSetRange",        qtSliderSetRangeNative,        3},
    {"sliderSetValue",        qtSliderSetValueNative,        2},
    {"sliderGetValue",        qtSliderGetValueNative,        1},
    {"sliderOnValueChanged",  qtSliderOnValueChangedNative,  2},
    {"spinboxCreate",         qtSpinBoxCreateNative,         -1},
    {"spinboxSetRange",       qtSpinBoxSetRangeNative,       3},
    {"spinboxGetValue",       qtSpinBoxGetValueNative,       1},
    {"spinboxSetValue",       qtSpinBoxSetValueNative,       2},
    {"spinboxOnChange",       qtSpinBoxOnChangeNative,       2},

    {"tabwidgetCreate",       qtTabWidgetCreateNative,       -1},
    {"tabwidgetAddTab",       qtTabWidgetAddTabNative,       3},
    {"tabwidgetSetCurrentIndex",qtTabWidgetSetCurrentIndexNative,2},

    {"stackedwidgetCreate",   qtStackedWidgetCreateNative,   -1},
    {"stackedwidgetAddWidget",qtStackedWidgetAddWidgetNative,2},
    {"stackedwidgetSetCurrentIndex",qtStackedWidgetSetCurrentIndexNative,2},

    {"groupboxCreate",        qtGroupBoxCreateNative,        -1},
    {"groupboxSetTitle",      qtGroupBoxSetTitleNative,      2},
    {"scrollareaCreate",      qtScrollAreaCreateNative,      -1},
    {"scrollareaSetWidget",   qtScrollAreaSetWidgetNative,   2},
    {"splitterCreate",        qtSplitterCreateNative,        -1},
    {"splitterAddWidget",     qtSplitterAddWidgetNative,     2},

    {"vboxCreate",            qtVBoxCreateNative,            -1},
    {"hboxCreate",            qtHBoxCreateNative,            -1},
    {"gridCreate",            qtGridCreateNative,            -1},
    {"formCreate",            qtFormCreateNative,            -1},
    {"layoutAddWidget",       qtLayoutAddWidgetNative,       2},
    {"layoutAddLayout",       qtLayoutAddLayoutNative,       2},
    {"layoutSetMargins",      qtLayoutSetMarginsNative,      5},
    {"layoutSetSpacing",      qtLayoutSetSpacingNative,      2},
    {"gridAddWidget",         qtGridAddWidgetNative,         -1},
    {"formAddRow",            qtFormAddRowNative,            3},

    {"listwidgetCreate",      qtListWidgetCreateNative,      -1},
    {"listwidgetAddItem",     qtListWidgetAddItemNative,     2},
    {"listwidgetGetCurrentItemText",qtListWidgetGetCurrentItemTextNative,1},
    {"listwidgetClear",       qtListWidgetClearNative,       1},

    {"treewidgetCreate",      qtTreeWidgetCreateNative,      -1},
    {"treewidgetSetHeaders",  qtTreeWidgetSetHeadersNative,  3},
    {"treewidgetAddItem",     qtTreeWidgetAddItemNative,     -1},

    {"tablewidgetCreate",     qtTableWidgetCreateNative,     -1},
    {"tablewidgetSetItem",    qtTableWidgetSetItemNative,    4},
    {"tablewidgetSetSize",    qtTableWidgetSetSizeNative,    3},
    {"tablewidgetSetHeaders", qtTableWidgetSetHeadersNative, 2},
    {"tablewidgetGetItem",    qtTableWidgetGetItemNative,    3},

    {"trayCreate",            qtTrayCreateNative,            -1},
    {"trayShow",              qtTrayShowNative,              1},
    {"trayHide",              qtTrayHideNative,              1},
    {"traySetIcon",           qtTraySetIconNative,           2},
    {"traySetMenu",           qtTraySetMenuNative,           2},
    {"trayShowMessage",       qtTrayShowMessageNative,       5},

    {"messageboxInfo",        qtMessageBoxInfoNative,        2},
    {"messageboxWarning",     qtMessageBoxWarningNative,     2},
    {"messageboxError",       qtMessageBoxErrorNative,       2},
    {"messageboxQuestion",    qtMessageBoxQuestionNative,    2},
    {"filedialogOpen",        qtFileDialogOpenNative,        -1},
    {"filedialogSave",        qtFileDialogSaveNative,        -1},
    {"filedialogGetDirectory",qtFileDialogGetDirectoryNative,-1},
    {"inputdialogGetText",    qtInputDialogGetTextNative,    2},

    {"timerCreate",           qtTimerCreateNative,           0},
    {"timerStart",            qtTimerStartNative,            2},
    {"timerStop",             qtTimerStopNative,             1},
    {"timerOnTimeout",        qtTimerOnTimeoutNative,        2},

    {"pixmapCreate",          qtPixmapCreateNative,          2},
    {"pixmapFill",            qtPixmapFillNative,            2},
    {"pixmapDestroy",         qtPixmapDestroyNative,         1},
    {"painterCreate",         qtPainterCreateNative,         1},
    {"painterSetPen",         qtPainterSetPenNative,         3},
    {"painterSetBrush",       qtPainterSetBrushNative,       2},
    {"painterDrawLine",       qtPainterDrawLineNative,       5},
    {"painterDrawRect",       qtPainterDrawRectNative,       5},
    {"painterDrawEllipse",    qtPainterDrawEllipseNative,    5},
    {"painterDrawText",       qtPainterDrawTextNative,       4},
    {"painterEnd",            qtPainterEndNative,            1},
    {"painterDrawRoundedRect",qtPainterDrawRoundedRectNative, 6},
    {"painterDrawArc",        qtPainterDrawArcNative,        7},
    {"painterDrawChord",      qtPainterDrawChordNative,      7},
    {"painterDrawPie",        qtPainterDrawPieNative,        7},
    {"painterDrawPolygon",    qtPainterDrawPolygonNative,    2},
    {"painterDrawPixmap",     qtPainterDrawPixmapNative,     4},
    {"painterSetPenStyle",    qtPainterSetPenStyleNative,    4},
    {"painterSetFont",        qtPainterSetFontNative,        5},

    {"pixmapSave",            qtPixmapSaveNative,            2},
    {"pixmapLoad",            qtPixmapLoadNative,            1},

    {"canvasCreate",          qtCanvasCreateNative,          -1},
    {"canvasBegin",           qtCanvasBeginNative,           1},
    {"canvasEnd",             qtCanvasEndNative,             1},
    {"canvasClear",           qtCanvasClearNative,           2},
    {"canvasSetSize",         qtCanvasSetSizeNative,         3},

    {"chartviewCreate",       qtChartViewCreateNative,       -1},
    {"chartviewSetChart",     qtChartViewSetChartNative,     2},
    {"chartCreate",           qtChartCreateNative,           0},
    {"chartSetTitle",         qtChartSetTitleNative,         2},
    {"chartAddSeries",        qtChartAddSeriesNative,        2},
    {"chartCreateDefaultAxes",qtChartCreateDefaultAxesNative,1},
    {"lineseriesCreate",      qtLineSeriesCreateNative,      0},
    {"lineseriesAppend",      qtLineSeriesAppendNative,      3},
    {"pieseriesCreate",       qtPieSeriesCreateNative,       0},
    {"pieseriesAppend",       qtPieSeriesAppendNative,       3},

    {"barseriesCreate",       qtBarSeriesCreateNative,       0},
    {"barsetCreate",          qtBarSetCreateNative,          1},
    {"barsetAppend",          qtBarSetAppendNative,          2},
    {"barseriesAppend",       qtBarSeriesAppendNative,       2},

    {"scatterseriesCreate",   qtScatterSeriesCreateNative,   0},
    {"scatterseriesAppend",   qtScatterSeriesAppendNative,   3},

    {"chartSetAxisTitle",     qtChartSetAxisTitleNative,     3},
    {"chartSetAxisRange",     qtChartSetAxisRangeNative,     4},

    {"widgetSetAcceptDrops",  qtWidgetSetAcceptDropsNative,  2},
    {"widgetOnDrop",          qtWidgetOnDropNative,          2},

    {"dialogCreate",          qtDialogCreateNative,          -1},
    {"dialogExec",            qtDialogExecNative,            1},

    {"doublespinboxCreate",         qtDoubleSpinBoxCreateNative,         -1},
    {"doublespinboxSetRange",       qtDoubleSpinBoxSetRangeNative,       3},
    {"doublespinboxSetValue",       qtDoubleSpinBoxSetValueNative,       2},
    {"doublespinboxGetValue",       qtDoubleSpinBoxGetValueNative,       1},
    {"doublespinboxSetDecimals",    qtDoubleSpinBoxSetDecimalsNative,    2},

    {"plaintexteditCreate",         qtPlainTextEditCreateNative,         -1},
    {"plaintexteditAppend",         qtPlainTextEditAppendNative,         2},
    {"plaintexteditClear",          qtPlainTextEditClearNative,          1},
    {"plaintexteditGetText",        qtPlainTextEditGetTextNative,        1},
    {"plaintexteditSetReadOnly",    qtPlainTextEditSetReadOnlyNative,    2},

    {NULL, NULL, 0}
};

extern "C" {
    DJAZAIR_EXPORT void init_qt_module(struct djazairVM *vm) {
        g_qt_vm = vm;
        djazair_register_native_module(vm, "_qt", qt_module_funcs);
    }
}
