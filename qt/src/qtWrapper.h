/**
 * [ Djazair Programming Language Project ]
 * Developer: Harizi Riyadh (hariziriyadh@gmail.com)
 *
 * qt_wrapper.h — Enterprise C API Bridge for Qt Framework
 * =================================================================
 * Native C-compatible wrapper around Qt5 / Qt6 C++ classes.
 */

#ifndef DJAZAIR_QT_WRAPPER_H
#define DJAZAIR_QT_WRAPPER_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque pointer handles for Qt C++ objects */
typedef void* QtAppHandle;
typedef void* QtWidgetHandle;
typedef void* QtMainWindowHandle;
typedef void* QtLayoutHandle;
typedef void* QtTimerHandle;
typedef void* QtTrayHandle;
typedef void* QtActionHandle;
typedef void* QtMenuHandle;
typedef void* QtPainterHandle;
typedef void* QtPixmapHandle;
typedef void* QtColorHandle;
typedef void* QtFontHandle;
typedef void* QtIconHandle;
typedef void* QtMediaPlayerHandle;
typedef void* QtChartHandle;
typedef void* QtChartViewHandle;
typedef void* QtSeriesHandle;


/* Callback function pointer types for Qt Signal / Event bridging */
typedef void (*QtVoidCallback)(void* user_data);
typedef void (*QtIntCallback)(int value, void* user_data);
typedef void (*QtStringCallback)(const char* text, void* user_data);
typedef void (*QtDoubleCallback)(double value, void* user_data);

/* Resource Cleanup Helpers */
void qt_object_track(void* handle);
bool qt_object_is_alive(void* handle);
void qt_object_delete(void* handle);
bool qt_painter_is_alive(void* handle);

/* ============================================================
 * Application Life Cycle & Global Theme
 * ============================================================ */
QtAppHandle qt_app_create(int argc, char** argv);
int         qt_app_exec(QtAppHandle app);
void        qt_app_process_events(QtAppHandle app);
void        qt_app_quit(QtAppHandle app);
void        qt_app_set_style(const char* style_name);
void        qt_app_destroy(QtAppHandle app);

/* ============================================================
 * Base QWidget Operations & Attributes
 * ============================================================ */
QtWidgetHandle qt_widget_create(QtWidgetHandle parent);
void           qt_widget_show(QtWidgetHandle widget);
void           qt_widget_hide(QtWidgetHandle widget);
void           qt_widget_set_title(QtWidgetHandle widget, const char* title);
void           qt_widget_set_geometry(QtWidgetHandle widget, int x, int y, int w, int h);
void           qt_widget_resize(QtWidgetHandle widget, int w, int h);
void           qt_widget_set_layout(QtWidgetHandle widget, QtLayoutHandle layout);
void           qt_widget_set_style_sheet(QtWidgetHandle widget, const char* css);
void           qt_widget_set_enabled(QtWidgetHandle widget, bool enabled);
void           qt_widget_set_visible(QtWidgetHandle widget, bool visible);
void           qt_widget_set_tooltip(QtWidgetHandle widget, const char* text);
void           qt_widget_update(QtWidgetHandle widget);
void           qt_widget_destroy(QtWidgetHandle widget);

/* Drag & Drop */
void           qt_widget_set_accept_drops(QtWidgetHandle widget, bool accept);
void           qt_widget_on_drop(QtWidgetHandle widget, QtStringCallback callback, void* user_data);

/* ============================================================
 * QMainWindow, MenuBar, Menu, Action, ToolBar, StatusBar
 * ============================================================ */
QtMainWindowHandle qt_mainwindow_create(void);
void               qt_mainwindow_set_central_widget(QtMainWindowHandle main_win, QtWidgetHandle widget);
void               qt_mainwindow_set_title(QtMainWindowHandle main_win, const char* title);
void               qt_mainwindow_resize(QtMainWindowHandle main_win, int w, int h);
void               qt_mainwindow_show(QtMainWindowHandle main_win);
QtMenuHandle       qt_mainwindow_add_menu(QtMainWindowHandle main_win, const char* title);
QtMenuHandle       qt_menu_add_submenu(QtMenuHandle menu, const char* title);
QtActionHandle     qt_menu_add_action(QtMenuHandle menu, const char* text, QtVoidCallback callback, void* user_data);
void               qt_menu_add_separator(QtMenuHandle menu);
QtWidgetHandle     qt_mainwindow_add_toolbar(QtMainWindowHandle main_win, const char* title);
void               qt_toolbar_add_action(QtWidgetHandle toolbar, const char* text, QtVoidCallback callback, void* user_data);
void               qt_mainwindow_set_status_message(QtMainWindowHandle main_win, const char* message, int timeout_ms);

/* ============================================================
 * Dynamic UI Designer Loader (QtUiTools & QUiLoader)
 * ============================================================ */
QtWidgetHandle qt_uiloader_load_file(const char* filepath, QtWidgetHandle parent);

/* ============================================================
 * Multimedia Engine (QtMultimedia & QMediaPlayer)
 * ============================================================ */
QtMediaPlayerHandle qt_mediaplayer_create(void);
void                qt_mediaplayer_set_media(QtMediaPlayerHandle player, const char* file_or_url);
void                qt_mediaplayer_play(QtMediaPlayerHandle player);
void                qt_mediaplayer_pause(QtMediaPlayerHandle player);
void                qt_mediaplayer_stop(QtMediaPlayerHandle player);
void                qt_mediaplayer_set_volume(QtMediaPlayerHandle player, int volume);
QtWidgetHandle      qt_videowidget_create(QtWidgetHandle parent);
void                qt_mediaplayer_set_video_output(QtMediaPlayerHandle player, QtWidgetHandle video_widget);

/* ============================================================
 * Basic Controls (Button, Label, LineEdit, TextEdit, CheckBox, Radio, Sliders)
 * ============================================================ */
QtWidgetHandle qt_button_create(const char* text, QtWidgetHandle parent);
void           qt_button_set_text(QtWidgetHandle btn, const char* text);
void           qt_button_on_click(QtWidgetHandle btn, QtVoidCallback callback, void* user_data);

QtWidgetHandle qt_label_create(const char* text, QtWidgetHandle parent);
void           qt_label_set_text(QtWidgetHandle label, const char* text);
const char*    qt_label_get_text(QtWidgetHandle label);
void           qt_label_set_alignment(QtWidgetHandle label, int align_flag);
void           qt_label_set_pixmap(QtWidgetHandle label, QtPixmapHandle pixmap);

/* ============================================================
 * Canvas & Drawing (QPainter & QPixmap)
 * ============================================================ */
QtPixmapHandle qt_pixmap_create(int w, int h);
void           qt_pixmap_fill(QtPixmapHandle pixmap, const char* color_hex);
void           qt_pixmap_destroy(QtPixmapHandle pixmap);

QtPainterHandle qt_painter_create(QtPixmapHandle pixmap);
void            qt_painter_set_pen(QtPainterHandle painter, const char* color_hex, int width);
void            qt_painter_set_brush(QtPainterHandle painter, const char* color_hex);
void            qt_painter_draw_line(QtPainterHandle painter, int x1, int y1, int x2, int y2);
void            qt_painter_draw_rect(QtPainterHandle painter, int x, int y, int w, int h);
void            qt_painter_draw_ellipse(QtPainterHandle painter, int x, int y, int w, int h);
void            qt_painter_draw_text(QtPainterHandle painter, int x, int y, const char* text);
void            qt_painter_end(QtPainterHandle painter);

/* Extended drawing primitives */
void            qt_painter_draw_rounded_rect(QtPainterHandle painter, int x, int y, int w, int h, int radius);
void            qt_painter_draw_arc(QtPainterHandle painter, int x, int y, int w, int h, int startAngle, int spanAngle);
void            qt_painter_draw_chord(QtPainterHandle painter, int x, int y, int w, int h, int startAngle, int spanAngle);
void            qt_painter_draw_pie(QtPainterHandle painter, int x, int y, int w, int h, int startAngle, int spanAngle);
void            qt_painter_draw_polygon(QtPainterHandle painter, const int* points, int count);
void            qt_painter_draw_pixmap(QtPainterHandle painter, QtPixmapHandle pixmap, int x, int y);
void            qt_painter_set_pen_style(QtPainterHandle painter, int style, int width, const char* color_hex);
void            qt_painter_set_font(QtPainterHandle painter, const char* family, int size, bool bold, bool italic);

/* Pixmap save/load */
bool            qt_pixmap_save(QtPixmapHandle pixmap, const char* path);
QtPixmapHandle  qt_pixmap_load(const char* path);

/* Canvas widget (double-buffered drawing area) */
QtWidgetHandle  qt_canvas_create(QtWidgetHandle parent);
QtPainterHandle qt_canvas_begin(QtWidgetHandle canvas);
void            qt_canvas_end(QtWidgetHandle canvas);
void            qt_canvas_clear(QtWidgetHandle canvas, const char* color_hex);
void            qt_canvas_set_size(QtWidgetHandle canvas, int w, int h);

QtWidgetHandle qt_lineedit_create(const char* text, QtWidgetHandle parent);
void           qt_lineedit_set_text(QtWidgetHandle line_edit, const char* text);
const char*    qt_lineedit_get_text(QtWidgetHandle line_edit);
void           qt_lineedit_set_placeholder(QtWidgetHandle line_edit, const char* text);
void           qt_lineedit_set_echo_mode(QtWidgetHandle line_edit, int mode);
void           qt_lineedit_on_change(QtWidgetHandle line_edit, QtStringCallback callback, void* user_data);

QtWidgetHandle qt_textedit_create(const char* text, QtWidgetHandle parent);
void           qt_textedit_set_text(QtWidgetHandle text_edit, const char* text);
const char*    qt_textedit_get_text(QtWidgetHandle text_edit);
void           qt_textedit_clear(QtWidgetHandle text_edit);
void           qt_textedit_append(QtWidgetHandle text_edit, const char* text);
void           qt_textedit_on_change(QtWidgetHandle text_edit, QtVoidCallback callback, void* user_data);

QtWidgetHandle qt_checkbox_create(const char* text, QtWidgetHandle parent);
void           qt_checkbox_set_checked(QtWidgetHandle cb, bool checked);
bool           qt_checkbox_is_checked(QtWidgetHandle cb);
void           qt_checkbox_on_toggled(QtWidgetHandle cb, QtIntCallback callback, void* user_data);

QtWidgetHandle qt_radiobutton_create(const char* text, QtWidgetHandle parent);
void           qt_radiobutton_set_checked(QtWidgetHandle rb, bool checked);
bool           qt_radiobutton_is_checked(QtWidgetHandle rb);

QtWidgetHandle qt_combobox_create(QtWidgetHandle parent);
void           qt_combobox_add_item(QtWidgetHandle combo, const char* text);
const char*    qt_combobox_get_current_text(QtWidgetHandle combo);
int            qt_combobox_get_current_index(QtWidgetHandle combo);
void           qt_combobox_set_current_index(QtWidgetHandle combo, int index);
void           qt_combobox_on_change(QtWidgetHandle combo, QtIntCallback callback, void* user_data);

QtWidgetHandle qt_progressbar_create(QtWidgetHandle parent);
void           qt_progressbar_set_range(QtWidgetHandle bar, int min, int max);
void           qt_progressbar_set_value(QtWidgetHandle bar, int value);
int            qt_progressbar_get_value(QtWidgetHandle bar);

QtWidgetHandle qt_slider_create(int orientation, QtWidgetHandle parent);
void           qt_slider_set_range(QtWidgetHandle slider, int min, int max);
void           qt_slider_set_value(QtWidgetHandle slider, int value);
int            qt_slider_get_value(QtWidgetHandle slider);
void           qt_slider_on_value_changed(QtWidgetHandle slider, QtIntCallback callback, void* user_data);

QtWidgetHandle qt_spinbox_create(QtWidgetHandle parent);
void           qt_spinbox_set_range(QtWidgetHandle spin, int min, int max);
void           qt_spinbox_set_value(QtWidgetHandle spin, int value);
int            qt_spinbox_get_value(QtWidgetHandle spin);
void           qt_spinbox_on_change(QtWidgetHandle spin, QtIntCallback callback, void* user_data);

/* ============================================================
 * Containers (TabWidget, StackedWidget, GroupBox, ScrollArea, Splitter)
 * ============================================================ */
QtWidgetHandle qt_tabwidget_create(QtWidgetHandle parent);
void           qt_tabwidget_add_tab(QtWidgetHandle tabs, QtWidgetHandle widget, const char* title);
void           qt_tabwidget_set_current_index(QtWidgetHandle tabs, int index);

QtWidgetHandle qt_stackedwidget_create(QtWidgetHandle parent);
int            qt_stackedwidget_add_widget(QtWidgetHandle stacked, QtWidgetHandle widget);
void           qt_stackedwidget_set_current_index(QtWidgetHandle stacked, int index);

QtWidgetHandle qt_groupbox_create(const char* title, QtWidgetHandle parent);
void           qt_groupbox_set_title(QtWidgetHandle group, const char* title);

QtWidgetHandle qt_scrollarea_create(QtWidgetHandle parent);
void           qt_scrollarea_set_widget(QtWidgetHandle scroll, QtWidgetHandle widget);

QtWidgetHandle qt_splitter_create(int orientation, QtWidgetHandle parent);
void           qt_splitter_add_widget(QtWidgetHandle splitter, QtWidgetHandle widget);

/* ============================================================
 * Layout Managers (VBox, HBox, Grid, Form)
 * ============================================================ */
QtLayoutHandle qt_vbox_create(QtWidgetHandle parent);
QtLayoutHandle qt_hbox_create(QtWidgetHandle parent);
QtLayoutHandle qt_grid_create(QtWidgetHandle parent);
QtLayoutHandle qt_form_create(QtWidgetHandle parent);

void           qt_layout_add_widget(QtLayoutHandle layout, QtWidgetHandle widget);
void           qt_layout_add_layout(QtLayoutHandle layout, QtLayoutHandle child_layout);
void           qt_layout_set_margins(QtLayoutHandle layout, int left, int top, int right, int bottom);
void           qt_layout_set_spacing(QtLayoutHandle layout, int spacing);
void           qt_grid_add_widget(QtLayoutHandle grid, QtWidgetHandle widget, int row, int col, int row_span, int col_span);
void           qt_form_add_row(QtLayoutHandle form, const char* label_text, QtWidgetHandle field_widget);

/* ============================================================
 * Complex Views (ListWidget, TreeWidget, TableWidget)
 * ============================================================ */
QtWidgetHandle qt_listwidget_create(QtWidgetHandle parent);
void           qt_listwidget_add_item(QtWidgetHandle list, const char* text);
const char*    qt_listwidget_get_current_item_text(QtWidgetHandle list);
void           qt_listwidget_clear(QtWidgetHandle list);

QtWidgetHandle qt_treewidget_create(QtWidgetHandle parent);
void           qt_treewidget_set_headers(QtWidgetHandle tree, const char** headers, int count);
void*          qt_treewidget_add_item(QtWidgetHandle tree, void* parent_item, const char** columns, int count);

QtWidgetHandle qt_tablewidget_create(int rows, int cols, QtWidgetHandle parent);
void           qt_tablewidget_set_size(QtWidgetHandle table, int rows, int cols);
void           qt_tablewidget_set_headers(QtWidgetHandle table, const char** headers, int count);
void           qt_tablewidget_set_item(QtWidgetHandle table, int row, int col, const char* text);
const char*    qt_tablewidget_get_item(QtWidgetHandle table, int row, int col);

/* ============================================================
 * System Tray Icon
 * ============================================================ */
QtTrayHandle   qt_tray_create(const char* tooltip);
void           qt_tray_show(QtTrayHandle tray);
void           qt_tray_hide(QtTrayHandle tray);
void           qt_tray_set_icon(QtTrayHandle tray, const char* icon_path);
void           qt_tray_set_menu(QtTrayHandle tray, QtMenuHandle menu);
void           qt_tray_show_message(QtTrayHandle tray, const char* title, const char* message, int icon_type, int timeout_ms);

/* ============================================================
 * QDialog (Modal Dialog)
 * ============================================================ */
QtWidgetHandle qt_dialog_create(QtWidgetHandle parent);
int           qt_dialog_exec(QtWidgetHandle dialog);

/* ============================================================
 * QDoubleSpinBox (Decimal Spin Control)
 * ============================================================ */
QtWidgetHandle qt_doublespinbox_create(QtWidgetHandle parent);
void           qt_doublespinbox_set_range(QtWidgetHandle spin, double min, double max);
void           qt_doublespinbox_set_value(QtWidgetHandle spin, double value);
double         qt_doublespinbox_get_value(QtWidgetHandle spin);
void           qt_doublespinbox_set_decimals(QtWidgetHandle spin, int prec);

/* ============================================================
 * QPlainTextEdit (Plain Text Editor)
 * ============================================================ */
QtWidgetHandle qt_plaintextedit_create(const char* text, QtWidgetHandle parent);
void           qt_plaintextedit_append(QtWidgetHandle edit, const char* text);
void           qt_plaintextedit_clear(QtWidgetHandle edit);
const char*    qt_plaintextedit_get_text(QtWidgetHandle edit);
void           qt_plaintextedit_set_read_only(QtWidgetHandle edit, bool ro);

/* ============================================================
 * Dialogs & Modals (MessageBox, FileDialog, InputDialog)
 * ============================================================ */
void        qt_messagebox_info(QtWidgetHandle parent, const char* title, const char* message);
void        qt_messagebox_warning(QtWidgetHandle parent, const char* title, const char* message);
void        qt_messagebox_error(QtWidgetHandle parent, const char* title, const char* message);
bool        qt_messagebox_question(QtWidgetHandle parent, const char* title, const char* message);

const char* qt_filedialog_get_open_file_name(QtWidgetHandle parent, const char* title, const char* filter);
const char* qt_filedialog_get_save_file_name(QtWidgetHandle parent, const char* title, const char* filter);
const char* qt_filedialog_get_existing_directory(QtWidgetHandle parent, const char* title);
const char* qt_inputdialog_get_text(QtWidgetHandle parent, const char* title, const char* label);

/* ============================================================
 * Timers
 * ============================================================ */
QtTimerHandle qt_timer_create(void);
void          qt_timer_start(QtTimerHandle timer, int msec);
void          qt_timer_stop(QtTimerHandle timer);
void          qt_timer_on_timeout(QtTimerHandle timer, QtVoidCallback callback, void* user_data);
void          qt_timer_destroy(QtTimerHandle timer);

/* ============================================================
 * Qt Charts
 * ============================================================ */
QtChartViewHandle qt_chartview_create(QtWidgetHandle parent);
void              qt_chartview_set_chart(QtChartViewHandle view, QtChartHandle chart);

QtChartHandle     qt_chart_create(void);
void              qt_chart_set_title(QtChartHandle chart, const char* title);
void              qt_chart_add_series(QtChartHandle chart, QtSeriesHandle series);
void              qt_chart_create_default_axes(QtChartHandle chart);

QtSeriesHandle    qt_lineseries_create(void);
void              qt_lineseries_append(QtSeriesHandle series, double x, double y);

QtSeriesHandle    qt_pieseries_create(void);
void              qt_pieseries_append(QtSeriesHandle series, const char* label, double value);

QtSeriesHandle    qt_barseries_create(void);
QtSeriesHandle    qt_barset_create(const char* label);
void              qt_barset_append(QtSeriesHandle barset, double value);
void              qt_barseries_append(QtSeriesHandle series, QtSeriesHandle barset);

QtSeriesHandle    qt_scatterseries_create(void);
void              qt_scatterseries_append(QtSeriesHandle series, double x, double y);

void              qt_chart_set_axis_title(QtChartHandle chart, int orientation, const char* title);
void              qt_chart_set_axis_range(QtChartHandle chart, int orientation, double min, double max);

#ifdef __cplusplus
}
#endif

#endif /* DJAZAIR_QT_WRAPPER_H */
