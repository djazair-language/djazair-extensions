/**
 * [ Djazair Programming Language Project ]
 * Developer: Harizi Riyadh (hariziriyadh@gmail.com)
 *
 * qt_wrapper.cpp — Enterprise C++ Implementation of Qt C API Bridge
 * =================================================================
 * Wraps Qt5 / Qt6 C++ classes into C-callable functions.
 */

#include "qt_wrapper.h"

#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QStackedWidget>
#include <QGroupBox>
#include <QScrollArea>
#include <QSplitter>
#include <QListWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QStyleFactory>

#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

static std::string g_temp_str_buffer;

static const char* copy_qstring_to_temp(const QString& qstr) {
    g_temp_str_buffer = qstr.toStdString();
    return g_temp_str_buffer.c_str();
}

extern "C" {

/* ============================================================
 * Application Life Cycle & Theme
 * ============================================================ */
QtAppHandle qt_app_create(int argc, char** argv) {
    static int fake_argc = 1;
    static char app_name[] = "DjazairQtApp";
    static char* fake_argv[] = { app_name, nullptr };

    int actual_argc = (argc > 0 && argv != nullptr) ? argc : fake_argc;
    char** actual_argv = (argc > 0 && argv != nullptr) ? argv : fake_argv;

    if (QCoreApplication::instance()) {
        return (QtAppHandle)QCoreApplication::instance();
    }
    return (QtAppHandle)new QApplication(actual_argc, actual_argv);
}

int qt_app_exec(QtAppHandle app) {
    if (!app) return -1;
    return QApplication::exec();
}

void qt_app_process_events(QtAppHandle app) {
    (void)app;
    QApplication::processEvents();
}

void qt_app_quit(QtAppHandle app) {
    (void)app;
    QApplication::quit();
}

void qt_app_set_style(const char* style_name) {
    if (style_name) {
        QApplication::setStyle(QStyleFactory::create(QString::fromUtf8(style_name)));
    }
}

void qt_app_destroy(QtAppHandle app) {
    if (app) {
        delete (QApplication*)app;
    }
}

/* ============================================================
 * Base QWidget Operations & Attributes
 * ============================================================ */
QtWidgetHandle qt_widget_create(QtWidgetHandle parent) {
    return (QtWidgetHandle)new QWidget((QWidget*)parent);
}

void qt_widget_show(QtWidgetHandle widget) {
    if (widget) ((QWidget*)widget)->show();
}

void qt_widget_hide(QtWidgetHandle widget) {
    if (widget) ((QWidget*)widget)->hide();
}

void qt_widget_set_title(QtWidgetHandle widget, const char* title) {
    if (widget && title) ((QWidget*)widget)->setWindowTitle(QString::fromUtf8(title));
}

void qt_widget_set_geometry(QtWidgetHandle widget, int x, int y, int w, int h) {
    if (widget) ((QWidget*)widget)->setGeometry(x, y, w, h);
}

void qt_widget_resize(QtWidgetHandle widget, int w, int h) {
    if (widget) ((QWidget*)widget)->resize(w, h);
}

void qt_widget_set_layout(QtWidgetHandle widget, QtLayoutHandle layout) {
    if (widget && layout) ((QWidget*)widget)->setLayout((QLayout*)layout);
}

void qt_widget_set_style_sheet(QtWidgetHandle widget, const char* css) {
    if (widget && css) ((QWidget*)widget)->setStyleSheet(QString::fromUtf8(css));
}

void qt_widget_set_enabled(QtWidgetHandle widget, bool enabled) {
    if (widget) ((QWidget*)widget)->setEnabled(enabled);
}

void qt_widget_set_visible(QtWidgetHandle widget, bool visible) {
    if (widget) ((QWidget*)widget)->setVisible(visible);
}

void qt_widget_set_tooltip(QtWidgetHandle widget, const char* text) {
    if (widget && text) ((QWidget*)widget)->setToolTip(QString::fromUtf8(text));
}

void qt_widget_update(QtWidgetHandle widget) {
    if (widget) ((QWidget*)widget)->update();
}

void qt_widget_destroy(QtWidgetHandle widget) {
    if (widget) delete (QWidget*)widget;
}

/* ============================================================
 * QMainWindow & Bars
 * ============================================================ */
QtMainWindowHandle qt_mainwindow_create(void) {
    return (QtMainWindowHandle)new QMainWindow();
}

void qt_mainwindow_set_central_widget(QtMainWindowHandle main_win, QtWidgetHandle widget) {
    if (main_win && widget) {
        ((QMainWindow*)main_win)->setCentralWidget((QWidget*)widget);
    }
}

void qt_mainwindow_set_title(QtMainWindowHandle main_win, const char* title) {
    if (main_win && title) {
        ((QMainWindow*)main_win)->setWindowTitle(QString::fromUtf8(title));
    }
}

void qt_mainwindow_resize(QtMainWindowHandle main_win, int w, int h) {
    if (main_win) ((QMainWindow*)main_win)->resize(w, h);
}

void qt_mainwindow_show(QtMainWindowHandle main_win) {
    if (main_win) ((QMainWindow*)main_win)->show();
}

QtMenuHandle qt_mainwindow_add_menu(QtMainWindowHandle main_win, const char* title) {
    if (!main_win || !title) return nullptr;
    QMenuBar* bar = ((QMainWindow*)main_win)->menuBar();
    return (QtMenuHandle)bar->addMenu(QString::fromUtf8(title));
}

QtActionHandle qt_menu_add_action(QtMenuHandle menu, const char* text, QtVoidCallback callback, void* user_data) {
    if (!menu || !text) return nullptr;
    QMenu* qmenu = (QMenu*)menu;
    QAction* action = qmenu->addAction(QString::fromUtf8(text));
    
    if (callback) {
        QObject::connect(action, &QAction::triggered, [callback, user_data]() {
            callback(user_data);
        });
    }
    return (QtActionHandle)action;
}

void qt_menu_add_separator(QtMenuHandle menu) {
    if (menu) ((QMenu*)menu)->addSeparator();
}

QtWidgetHandle qt_mainwindow_add_toolbar(QtMainWindowHandle main_win, const char* title) {
    if (!main_win) return nullptr;
    QToolBar* bar = ((QMainWindow*)main_win)->addToolBar(QString::fromUtf8(title ? title : "ToolBar"));
    return (QtWidgetHandle)bar;
}

void qt_toolbar_add_action(QtWidgetHandle toolbar, const char* text, QtVoidCallback callback, void* user_data) {
    if (!toolbar || !text) return;
    QToolBar* bar = (QToolBar*)toolbar;
    QAction* action = bar->addAction(QString::fromUtf8(text));
    if (callback) {
        QObject::connect(action, &QAction::triggered, [callback, user_data]() {
            callback(user_data);
        });
    }
}

void qt_mainwindow_set_status_message(QtMainWindowHandle main_win, const char* message, int timeout_ms) {
    if (main_win && message) {
        ((QMainWindow*)main_win)->statusBar()->showMessage(QString::fromUtf8(message), timeout_ms);
    }
}

/* ============================================================
 * Basic Controls
 * ============================================================ */
QtWidgetHandle qt_button_create(const char* text, QtWidgetHandle parent) {
    return (QtWidgetHandle)new QPushButton(QString::fromUtf8(text ? text : ""), (QWidget*)parent);
}

void qt_button_set_text(QtWidgetHandle btn, const char* text) {
    if (btn && text) ((QPushButton*)btn)->setText(QString::fromUtf8(text));
}

void qt_button_on_click(QtWidgetHandle btn, QtVoidCallback callback, void* user_data) {
    if (!btn || !callback) return;
    QPushButton* qbtn = (QPushButton*)btn;
    QObject::connect(qbtn, &QPushButton::clicked, [callback, user_data]() {
        callback(user_data);
    });
}

QtWidgetHandle qt_label_create(const char* text, QtWidgetHandle parent) {
    return (QtWidgetHandle)new QLabel(QString::fromUtf8(text ? text : ""), (QWidget*)parent);
}

void qt_label_set_text(QtWidgetHandle label, const char* text) {
    if (label && text) ((QLabel*)label)->setText(QString::fromUtf8(text));
}

const char* qt_label_get_text(QtWidgetHandle label) {
    if (!label) return "";
    return copy_qstring_to_temp(((QLabel*)label)->text());
}

void qt_label_set_alignment(QtWidgetHandle label, int align_flag) {
    if (label) ((QLabel*)label)->setAlignment((Qt::AlignmentFlag)align_flag);
}

void qt_label_set_pixmap(QtWidgetHandle label, QtPixmapHandle pixmap) {
    if (label && pixmap) ((QLabel*)label)->setPixmap(*(QPixmap*)pixmap);
}

QtWidgetHandle qt_lineedit_create(const char* text, QtWidgetHandle parent) {
    return (QtWidgetHandle)new QLineEdit(QString::fromUtf8(text ? text : ""), (QWidget*)parent);
}

void qt_lineedit_set_text(QtWidgetHandle line_edit, const char* text) {
    if (line_edit && text) ((QLineEdit*)line_edit)->setText(QString::fromUtf8(text));
}

const char* qt_lineedit_get_text(QtWidgetHandle line_edit) {
    if (!line_edit) return "";
    return copy_qstring_to_temp(((QLineEdit*)line_edit)->text());
}

void qt_lineedit_set_placeholder(QtWidgetHandle line_edit, const char* text) {
    if (line_edit && text) ((QLineEdit*)line_edit)->setPlaceholderText(QString::fromUtf8(text));
}

void qt_lineedit_set_echo_mode(QtWidgetHandle line_edit, int mode) {
    if (line_edit) ((QLineEdit*)line_edit)->setEchoMode((QLineEdit::EchoMode)mode);
}

void qt_lineedit_on_change(QtWidgetHandle line_edit, QtStringCallback callback, void* user_data) {
    if (!line_edit || !callback) return;
    QLineEdit* qedit = (QLineEdit*)line_edit;
    QObject::connect(qedit, &QLineEdit::textChanged, [callback, user_data](const QString& qstr) {
        callback(qstr.toUtf8().constData(), user_data);
    });
}

QtWidgetHandle qt_textedit_create(const char* text, QtWidgetHandle parent) {
    return (QtWidgetHandle)new QTextEdit(QString::fromUtf8(text ? text : ""), (QWidget*)parent);
}

void qt_textedit_set_text(QtWidgetHandle text_edit, const char* text) {
    if (text_edit && text) ((QTextEdit*)text_edit)->setText(QString::fromUtf8(text));
}

const char* qt_textedit_get_text(QtWidgetHandle text_edit) {
    if (!text_edit) return "";
    return copy_qstring_to_temp(((QTextEdit*)text_edit)->toPlainText());
}

void qt_textedit_clear(QtWidgetHandle text_edit) {
    if (text_edit) ((QTextEdit*)text_edit)->clear();
}

void qt_textedit_append(QtWidgetHandle text_edit, const char* text) {
    if (text_edit && text) ((QTextEdit*)text_edit)->append(QString::fromUtf8(text));
}

void qt_textedit_on_change(QtWidgetHandle text_edit, QtVoidCallback callback, void* user_data) {
    if (!text_edit || !callback) return;
    QTextEdit* qedit = (QTextEdit*)text_edit;
    QObject::connect(qedit, &QTextEdit::textChanged, [callback, user_data]() {
        callback(user_data);
    });
}

QtWidgetHandle qt_checkbox_create(const char* text, QtWidgetHandle parent) {
    return (QtWidgetHandle)new QCheckBox(QString::fromUtf8(text ? text : ""), (QWidget*)parent);
}

void qt_checkbox_set_checked(QtWidgetHandle cb, bool checked) {
    if (cb) ((QCheckBox*)cb)->setChecked(checked);
}

bool qt_checkbox_is_checked(QtWidgetHandle cb) {
    return cb ? ((QCheckBox*)cb)->isChecked() : false;
}

void qt_checkbox_on_toggled(QtWidgetHandle cb, QtIntCallback callback, void* user_data) {
    if (!cb || !callback) return;
    QCheckBox* qcb = (QCheckBox*)cb;
    QObject::connect(qcb, &QCheckBox::toggled, [callback, user_data](bool checked) {
        callback(checked ? 1 : 0, user_data);
    });
}

QtWidgetHandle qt_radiobutton_create(const char* text, QtWidgetHandle parent) {
    return (QtWidgetHandle)new QRadioButton(QString::fromUtf8(text ? text : ""), (QWidget*)parent);
}

void qt_radiobutton_set_checked(QtWidgetHandle rb, bool checked) {
    if (rb) ((QRadioButton*)rb)->setChecked(checked);
}

bool qt_radiobutton_is_checked(QtWidgetHandle rb) {
    return rb ? ((QRadioButton*)rb)->isChecked() : false;
}

QtWidgetHandle qt_combobox_create(QtWidgetHandle parent) {
    return (QtWidgetHandle)new QComboBox((QWidget*)parent);
}

void qt_combobox_add_item(QtWidgetHandle combo, const char* text) {
    if (combo && text) ((QComboBox*)combo)->addItem(QString::fromUtf8(text));
}

const char* qt_combobox_get_current_text(QtWidgetHandle combo) {
    if (!combo) return "";
    return copy_qstring_to_temp(((QComboBox*)combo)->currentText());
}

int qt_combobox_get_current_index(QtWidgetHandle combo) {
    return combo ? ((QComboBox*)combo)->currentIndex() : -1;
}

void qt_combobox_set_current_index(QtWidgetHandle combo, int index) {
    if (combo) ((QComboBox*)combo)->setCurrentIndex(index);
}

void qt_combobox_on_change(QtWidgetHandle combo, QtIntCallback callback, void* user_data) {
    if (!combo || !callback) return;
    QComboBox* qcombo = (QComboBox*)combo;
    QObject::connect(qcombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [callback, user_data](int idx) {
        callback(idx, user_data);
    });
}

QtWidgetHandle qt_progressbar_create(QtWidgetHandle parent) {
    return (QtWidgetHandle)new QProgressBar((QWidget*)parent);
}

void qt_progressbar_set_range(QtWidgetHandle bar, int min, int max) {
    if (bar) ((QProgressBar*)bar)->setRange(min, max);
}

void qt_progressbar_set_value(QtWidgetHandle bar, int value) {
    if (bar) ((QProgressBar*)bar)->setValue(value);
}

int qt_progressbar_get_value(QtWidgetHandle bar) {
    return bar ? ((QProgressBar*)bar)->value() : 0;
}

QtWidgetHandle qt_slider_create(int orientation, QtWidgetHandle parent) {
    Qt::Orientation qt_orient = (orientation == 1) ? Qt::Vertical : Qt::Horizontal;
    return (QtWidgetHandle)new QSlider(qt_orient, (QWidget*)parent);
}

void qt_slider_set_range(QtWidgetHandle slider, int min, int max) {
    if (slider) ((QSlider*)slider)->setRange(min, max);
}

void qt_slider_set_value(QtWidgetHandle slider, int value) {
    if (slider) ((QSlider*)slider)->setValue(value);
}

int qt_slider_get_value(QtWidgetHandle slider) {
    return slider ? ((QSlider*)slider)->value() : 0;
}

void qt_slider_on_value_changed(QtWidgetHandle slider, QtIntCallback callback, void* user_data) {
    if (!slider || !callback) return;
    QSlider* qslider = (QSlider*)slider;
    QObject::connect(qslider, &QSlider::valueChanged, [callback, user_data](int val) {
        callback(val, user_data);
    });
}

QtWidgetHandle qt_spinbox_create(QtWidgetHandle parent) {
    return (QtWidgetHandle)new QSpinBox((QWidget*)parent);
}

void qt_spinbox_set_range(QtWidgetHandle spin, int min, int max) {
    if (spin) ((QSpinBox*)spin)->setRange(min, max);
}

void qt_spinbox_set_value(QtWidgetHandle spin, int value) {
    if (spin) ((QSpinBox*)spin)->setValue(value);
}

int qt_spinbox_get_value(QtWidgetHandle spin) {
    return spin ? ((QSpinBox*)spin)->value() : 0;
}

void qt_spinbox_on_change(QtWidgetHandle spin, QtIntCallback callback, void* user_data) {
    if (!spin || !callback) return;
    QSpinBox* qspin = (QSpinBox*)spin;
    QObject::connect(qspin, QOverload<int>::of(&QSpinBox::valueChanged), [callback, user_data](int val) {
        callback(val, user_data);
    });
}

/* ============================================================
 * Containers
 * ============================================================ */
QtWidgetHandle qt_tabwidget_create(QtWidgetHandle parent) {
    return (QtWidgetHandle)new QTabWidget((QWidget*)parent);
}

void qt_tabwidget_add_tab(QtWidgetHandle tabs, QtWidgetHandle widget, const char* title) {
    if (tabs && widget && title) {
        ((QTabWidget*)tabs)->addTab((QWidget*)widget, QString::fromUtf8(title));
    }
}

void qt_tabwidget_set_current_index(QtWidgetHandle tabs, int index) {
    if (tabs) ((QTabWidget*)tabs)->setCurrentIndex(index);
}

QtWidgetHandle qt_stackedwidget_create(QtWidgetHandle parent) {
    return (QtWidgetHandle)new QStackedWidget((QWidget*)parent);
}

int qt_stackedwidget_add_widget(QtWidgetHandle stacked, QtWidgetHandle widget) {
    if (stacked && widget) {
        return ((QStackedWidget*)stacked)->addWidget((QWidget*)widget);
    }
    return -1;
}

void qt_stackedwidget_set_current_index(QtWidgetHandle stacked, int index) {
    if (stacked) ((QStackedWidget*)stacked)->setCurrentIndex(index);
}

QtWidgetHandle qt_groupbox_create(const char* title, QtWidgetHandle parent) {
    return (QtWidgetHandle)new QGroupBox(QString::fromUtf8(title ? title : ""), (QWidget*)parent);
}

void qt_groupbox_set_title(QtWidgetHandle group, const char* title) {
    if (group && title) ((QGroupBox*)group)->setTitle(QString::fromUtf8(title));
}

QtWidgetHandle qt_scrollarea_create(QtWidgetHandle parent) {
    QScrollArea* area = new QScrollArea((QWidget*)parent);
    area->setWidgetResizable(true);
    return (QtWidgetHandle)area;
}

void qt_scrollarea_set_widget(QtWidgetHandle scroll, QtWidgetHandle widget) {
    if (scroll && widget) ((QScrollArea*)scroll)->setWidget((QWidget*)widget);
}

QtWidgetHandle qt_splitter_create(int orientation, QtWidgetHandle parent) {
    Qt::Orientation qt_orient = (orientation == 1) ? Qt::Vertical : Qt::Horizontal;
    return (QtWidgetHandle)new QSplitter(qt_orient, (QWidget*)parent);
}

void qt_splitter_add_widget(QtWidgetHandle splitter, QtWidgetHandle widget) {
    if (splitter && widget) ((QSplitter*)splitter)->addWidget((QWidget*)widget);
}

/* ============================================================
 * Layout Managers
 * ============================================================ */
QtLayoutHandle qt_vbox_create(QtWidgetHandle parent) {
    return (QtLayoutHandle)new QVBoxLayout((QWidget*)parent);
}

QtLayoutHandle qt_hbox_create(QtWidgetHandle parent) {
    return (QtLayoutHandle)new QHBoxLayout((QWidget*)parent);
}

QtLayoutHandle qt_grid_create(QtWidgetHandle parent) {
    return (QtLayoutHandle)new QGridLayout((QWidget*)parent);
}

QtLayoutHandle qt_form_create(QtWidgetHandle parent) {
    return (QtLayoutHandle)new QFormLayout((QWidget*)parent);
}

void qt_layout_add_widget(QtLayoutHandle layout, QtWidgetHandle widget) {
    if (layout && widget) ((QLayout*)layout)->addWidget((QWidget*)widget);
}

void qt_layout_add_layout(QtLayoutHandle layout, QtLayoutHandle child_layout) {
    if (layout && child_layout) ((QLayout*)layout)->addItem((QLayout*)child_layout);
}

void qt_layout_set_margins(QtLayoutHandle layout, int left, int top, int right, int bottom) {
    if (layout) ((QLayout*)layout)->setContentsMargins(left, top, right, bottom);
}

void qt_layout_set_spacing(QtLayoutHandle layout, int spacing) {
    if (layout) ((QLayout*)layout)->setSpacing(spacing);
}

void qt_grid_add_widget(QtLayoutHandle grid, QtWidgetHandle widget, int row, int col, int row_span, int col_span) {
    if (grid && widget) ((QGridLayout*)grid)->addWidget((QWidget*)widget, row, col, row_span, col_span);
}

void qt_form_add_row(QtLayoutHandle form, const char* label_text, QtWidgetHandle field_widget) {
    if (form && field_widget) {
        ((QFormLayout*)form)->addRow(QString::fromUtf8(label_text ? label_text : ""), (QWidget*)field_widget);
    }
}

/* ============================================================
 * Complex Views
 * ============================================================ */
QtWidgetHandle qt_listwidget_create(QtWidgetHandle parent) {
    return (QtWidgetHandle)new QListWidget((QWidget*)parent);
}

void qt_listwidget_add_item(QtWidgetHandle list, const char* text) {
    if (list && text) ((QListWidget*)list)->addItem(QString::fromUtf8(text));
}

const char* qt_listwidget_get_current_item_text(QtWidgetHandle list) {
    if (!list) return "";
    QListWidgetItem* item = ((QListWidget*)list)->currentItem();
    return item ? copy_qstring_to_temp(item->text()) : "";
}

void qt_listwidget_clear(QtWidgetHandle list) {
    if (list) ((QListWidget*)list)->clear();
}

QtWidgetHandle qt_treewidget_create(QtWidgetHandle parent) {
    return (QtWidgetHandle)new QTreeWidget((QWidget*)parent);
}

void qt_treewidget_set_headers(QtWidgetHandle tree, const char** headers, int count) {
    if (!tree || !headers) return;
    QStringList qheaders;
    for (int i = 0; i < count; ++i) {
        qheaders << QString::fromUtf8(headers[i]);
    }
    ((QTreeWidget*)tree)->setHeaderLabels(qheaders);
}

void* qt_treewidget_add_item(QtWidgetHandle tree, void* parent_item, const char** columns, int count) {
    if (!tree || !columns) return nullptr;
    QStringList qcols;
    for (int i = 0; i < count; ++i) {
        qcols << QString::fromUtf8(columns[i]);
    }

    if (parent_item) {
        QTreeWidgetItem* child = new QTreeWidgetItem((QTreeWidgetItem*)parent_item, qcols);
        return (void*)child;
    } else {
        QTreeWidgetItem* item = new QTreeWidgetItem((QTreeWidget*)tree, qcols);
        return (void*)item;
    }
}

QtWidgetHandle qt_tablewidget_create(int rows, int cols, QtWidgetHandle parent) {
    return (QtWidgetHandle)new QTableWidget(rows, cols, (QWidget*)parent);
}

void qt_tablewidget_set_size(QtWidgetHandle table, int rows, int cols) {
    if (table) {
        ((QTableWidget*)table)->setRowCount(rows);
        ((QTableWidget*)table)->setColumnCount(cols);
    }
}

void qt_tablewidget_set_headers(QtWidgetHandle table, const char** headers, int count) {
    if (!table || !headers) return;
    QStringList qheaders;
    for (int i = 0; i < count; ++i) {
        qheaders << QString::fromUtf8(headers[i]);
    }
    ((QTableWidget*)table)->setHorizontalHeaderLabels(qheaders);
}

void qt_tablewidget_set_item(QtWidgetHandle table, int row, int col, const char* text) {
    if (!table || !text) return;
    ((QTableWidget*)table)->setItem(row, col, new QTableWidgetItem(QString::fromUtf8(text)));
}

const char* qt_tablewidget_get_item(QtWidgetHandle table, int row, int col) {
    if (!table) return "";
    QTableWidgetItem* item = ((QTableWidget*)table)->item(row, col);
    return item ? copy_qstring_to_temp(item->text()) : "";
}

/* ============================================================
 * System Tray Icon
 * ============================================================ */
QtTrayHandle qt_tray_create(const char* tooltip) {
    QSystemTrayIcon* tray = new QSystemTrayIcon();
    if (tooltip) tray->setToolTip(QString::fromUtf8(tooltip));
    return (QtTrayHandle)tray;
}

void qt_tray_show(QtTrayHandle tray) {
    if (tray) ((QSystemTrayIcon*)tray)->show();
}

void qt_tray_hide(QtTrayHandle tray) {
    if (tray) ((QSystemTrayIcon*)tray)->hide();
}

void qt_tray_set_menu(QtTrayHandle tray, QtMenuHandle menu) {
    if (tray && menu) ((QSystemTrayIcon*)tray)->setContextMenu((QMenu*)menu);
}

void qt_tray_show_message(QtTrayHandle tray, const char* title, const char* message, int icon_type, int timeout_ms) {
    if (tray && title && message) {
        ((QSystemTrayIcon*)tray)->showMessage(
            QString::fromUtf8(title),
            QString::fromUtf8(message),
            (QSystemTrayIcon::MessageIcon)icon_type,
            timeout_ms
        );
    }
}

/* ============================================================
 * Dialogs & Message Boxes
 * ============================================================ */
void qt_messagebox_info(QtWidgetHandle parent, const char* title, const char* message) {
    QMessageBox::information((QWidget*)parent, QString::fromUtf8(title ? title : "Info"), QString::fromUtf8(message ? message : ""));
}

void qt_messagebox_warning(QtWidgetHandle parent, const char* title, const char* message) {
    QMessageBox::warning((QWidget*)parent, QString::fromUtf8(title ? title : "Warning"), QString::fromUtf8(message ? message : ""));
}

void qt_messagebox_error(QtWidgetHandle parent, const char* title, const char* message) {
    QMessageBox::critical((QWidget*)parent, QString::fromUtf8(title ? title : "Error"), QString::fromUtf8(message ? message : ""));
}

bool qt_messagebox_question(QtWidgetHandle parent, const char* title, const char* message) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        (QWidget*)parent, 
        QString::fromUtf8(title ? title : "Question"), 
        QString::fromUtf8(message ? message : ""),
        QMessageBox::Yes | QMessageBox::No
    );
    return (reply == QMessageBox::Yes);
}

const char* qt_filedialog_get_open_file_name(QtWidgetHandle parent, const char* title, const char* filter) {
    QString fileName = QFileDialog::getOpenFileName(
        (QWidget*)parent,
        QString::fromUtf8(title ? title : "Open File"),
        QString(),
        QString::fromUtf8(filter ? filter : "All Files (*.*)")
    );
    return copy_qstring_to_temp(fileName);
}

const char* qt_filedialog_get_save_file_name(QtWidgetHandle parent, const char* title, const char* filter) {
    QString fileName = QFileDialog::getSaveFileName(
        (QWidget*)parent,
        QString::fromUtf8(title ? title : "Save File"),
        QString(),
        QString::fromUtf8(filter ? filter : "All Files (*.*)")
    );
    return copy_qstring_to_temp(fileName);
}

const char* qt_filedialog_get_existing_directory(QtWidgetHandle parent, const char* title) {
    QString dirPath = QFileDialog::getExistingDirectory(
        (QWidget*)parent,
        QString::fromUtf8(title ? title : "Select Directory")
    );
    return copy_qstring_to_temp(dirPath);
}

const char* qt_inputdialog_get_text(QtWidgetHandle parent, const char* title, const char* label) {
    bool ok = false;
    QString text = QInputDialog::getText(
        (QWidget*)parent,
        QString::fromUtf8(title ? title : "Input"),
        QString::fromUtf8(label ? label : "Enter value:"),
        QLineEdit::Normal,
        QString(),
        &ok
    );
    return ok ? copy_qstring_to_temp(text) : "";
}

/* ============================================================
 * Timers
 * ============================================================ */
QtTimerHandle qt_timer_create(void) {
    return (QtTimerHandle)new QTimer();
}

void qt_timer_start(QtTimerHandle timer, int msec) {
    if (timer) ((QTimer*)timer)->start(msec);
}

void qt_timer_stop(QtTimerHandle timer) {
    if (timer) ((QTimer*)timer)->stop();
}

void qt_timer_on_timeout(QtTimerHandle timer, QtVoidCallback callback, void* user_data) {
    if (!timer || !callback) return;
    QTimer* qtimer = (QTimer*)timer;
    QObject::connect(qtimer, &QTimer::timeout, [callback, user_data]() {
        callback(user_data);
    });
}

void qt_timer_destroy(QtTimerHandle timer) {
    if (timer) delete (QTimer*)timer;
}

} // extern "C"
