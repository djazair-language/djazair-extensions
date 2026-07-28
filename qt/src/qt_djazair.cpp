/**
 * [ Djazair Programming Language Project ]
 * Developer: Harizi Riyadh (hariziriyadh@gmail.com)
 *
 * qt_djazair.cpp — Enterprise Djazair VM Native Bindings for Qt Extension
 * =================================================================
 * Exposes native C++ Qt operations to Djazair scripts via djazair_api.h.
 */

#include "qt_wrapper.h"

extern "C" {
#include "djazair_api.h"
}

#include <stdio.h>
#include <stdlib.h>

static void qtResourceFinalizer(void* ptr) {
    if (!ptr) return;
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

/* ============================================================
 * QMainWindow, ToolBar, StatusBar
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

DJAZAIR_FUNC(qtMainWindowAddToolBarNative) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
    QtMainWindowHandle main_win = (QtMainWindowHandle)get_qt_handle(vm, args, 0);
    if (!main_win) return NULL_VAL;
    QtWidgetHandle bar = qt_mainwindow_add_toolbar(main_win, djazair_get_str(args, 1));
    return djazair_new_resource_with_finalizer(vm, bar, "QToolBar", qtResourceFinalizer);
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
    if (argCount > 1 && djazair_is_resource(args[1])) parent = (QtWidgetHandle)djazair_get_resource(vm, args[0]);
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

DJAZAIR_FUNC(qtCheckBoxCreateNative) {
    const char* text = (argCount > 0 && djazair_is_string(args[0])) ? djazair_get_str(args, 0) : "";
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle cb = qt_checkbox_create(text, parent);
    return djazair_new_resource_with_finalizer(vm, cb, "QCheckBox", qtResourceFinalizer);
}

DJAZAIR_FUNC(qtRadioButtonCreateNative) {
    const char* text = (argCount > 0 && djazair_is_string(args[0])) ? djazair_get_str(args, 0) : "";
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle rb = qt_radiobutton_create(text, parent);
    return djazair_new_resource_with_finalizer(vm, rb, "QRadioButton", qtResourceFinalizer);
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

DJAZAIR_FUNC(qtGroupBoxCreateNative) {
    const char* title = (argCount > 0 && djazair_is_string(args[0])) ? djazair_get_str(args, 0) : "";
    QtWidgetHandle parent = (argCount > 1 && djazair_is_resource(args[1])) ? (QtWidgetHandle)djazair_get_resource(vm, args[1]) : NULL;
    QtWidgetHandle group = qt_groupbox_create(title, parent);
    return djazair_new_resource_with_finalizer(vm, group, "QGroupBox", qtResourceFinalizer);
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
 * Layout Managers
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
    return djazair_new_resource_with_finalizer(vm, itemPtr, "QTreeWidgetItem", qtResourceFinalizer);
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

/* ============================================================
 * System Tray & Dialogs
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

DJAZAIR_FUNC(qtMessageBoxInfoNative) {
    djazair_check_args(2, argCount);
    qt_messagebox_info(NULL, djazair_get_str(args, 0), djazair_get_str(args, 1));
    return djazair_null();
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
 * Dispatch Method Table
 * ============================================================ */
static NativeMethod qt_module_funcs[] = {
    {"appCreate",             qtAppCreateNative,             -1},
    {"appExec",               qtAppExecNative,               1},
    {"appProcessEvents",      qtAppProcessEventsNative,      1},
    {"appQuit",               qtAppQuitNative,               1},
    {"appSetStyle",           qtAppSetStyleNative,           1},

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

    {"mainwindowCreate",      qtMainWindowCreateNative,      0},
    {"mainwindowSetCentral",  qtMainWindowSetCentralWidgetNative, 2},
    {"mainwindowSetTitle",    qtMainWindowSetTitleNative,    2},
    {"mainwindowResize",      qtMainWindowResizeNative,      3},
    {"mainwindowShow",        qtMainWindowShowNative,        1},
    {"mainwindowAddMenu",     qtMainWindowAddMenuNative,     2},
    {"mainwindowAddToolBar",  qtMainWindowAddToolBarNative,  2},
    {"mainwindowSetStatus",   qtMainWindowSetStatusMessageNative, 3},

    {"buttonCreate",          qtButtonCreateNative,          -1},
    {"buttonSetText",         qtButtonSetTextNative,         2},

    {"labelCreate",           qtLabelCreateNative,           -1},
    {"labelSetText",          qtLabelSetTextNative,          2},
    {"labelGetText",          qtLabelGetTextNative,          1},

    {"lineeditCreate",        qtLineEditCreateNative,        -1},
    {"lineeditSetText",       qtLineEditSetTextNative,       2},
    {"lineeditGetText",       qtLineEditGetTextNative,       1},
    {"lineeditSetEchoMode",   qtLineEditSetEchoModeNative,   2},

    {"texteditCreate",        qtTextEditCreateNative,        -1},
    {"texteditSetText",       qtTextEditSetTextNative,       2},
    {"texteditGetText",       qtTextEditGetTextNative,       1},

    {"checkboxCreate",        qtCheckBoxCreateNative,        -1},
    {"radiobuttonCreate",     qtRadioButtonCreateNative,     -1},
    {"comboboxCreate",        qtComboBoxCreateNative,        -1},
    {"comboboxAddItem",       qtComboBoxAddItemNative,       2},
    {"progressbarCreate",     qtProgressBarCreateNative,     -1},
    {"progressbarSetRange",   qtProgressBarSetRangeNative,   3},
    {"progressbarSetValue",   qtProgressBarSetValueNative,   2},
    {"sliderCreate",          qtSliderCreateNative,          -1},
    {"sliderSetRange",        qtSliderSetRangeNative,        3},
    {"sliderSetValue",        qtSliderSetValueNative,        2},
    {"spinboxCreate",         qtSpinBoxCreateNative,         -1},
    {"spinboxSetRange",       qtSpinBoxSetRangeNative,       3},

    {"tabwidgetCreate",       qtTabWidgetCreateNative,       -1},
    {"tabwidgetAddTab",       qtTabWidgetAddTabNative,       3},

    {"stackedwidgetCreate",   qtStackedWidgetCreateNative,   -1},
    {"stackedwidgetAddWidget",qtStackedWidgetAddWidgetNative,2},

    {"groupboxCreate",        qtGroupBoxCreateNative,        -1},
    {"scrollareaCreate",      qtScrollAreaCreateNative,      -1},
    {"scrollareaSetWidget",   qtScrollAreaSetWidgetNative,   2},
    {"splitterCreate",        qtSplitterCreateNative,        -1},
    {"splitterAddWidget",     qtSplitterAddWidgetNative,     2},

    {"vboxCreate",            qtVBoxCreateNative,            -1},
    {"hboxCreate",            qtHBoxCreateNative,            -1},
    {"gridCreate",            qtGridCreateNative,            -1},
    {"formCreate",            qtFormCreateNative,            -1},
    {"layoutAddWidget",       qtLayoutAddWidgetNative,       2},
    {"gridAddWidget",         qtGridAddWidgetNative,         -1},
    {"formAddRow",            qtFormAddRowNative,            3},

    {"listwidgetCreate",      qtListWidgetCreateNative,      -1},
    {"listwidgetAddItem",     qtListWidgetAddItemNative,     2},

    {"treewidgetCreate",      qtTreeWidgetCreateNative,      -1},
    {"treewidgetSetHeaders",  qtTreeWidgetSetHeadersNative,  3},
    {"treewidgetAddItem",     qtTreeWidgetAddItemNative,     -1},

    {"tablewidgetCreate",     qtTableWidgetCreateNative,     -1},
    {"tablewidgetSetItem",    qtTableWidgetSetItemNative,    4},

    {"trayCreate",            qtTrayCreateNative,            -1},
    {"trayShow",              qtTrayShowNative,              1},
    {"trayShowMessage",       qtTrayShowMessageNative,       5},

    {"messageboxInfo",        qtMessageBoxInfoNative,        2},
    {"filedialogOpen",        qtFileDialogOpenNative,        -1},
    {"filedialogSave",        qtFileDialogSaveNative,        -1},
    {"filedialogGetDirectory",qtFileDialogGetDirectoryNative,-1},
    {"inputdialogGetText",    qtInputDialogGetTextNative,    2},

    {NULL, NULL, 0}
};

extern "C" {
    DJAZAIR_EXTENSION(qt, qt_module_funcs)
}
