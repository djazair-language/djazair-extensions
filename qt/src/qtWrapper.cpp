/**
 * [ Djazair Programming Language Project ]
 * Developer: Harizi Riyadh (hariziriyadh@gmail.com)
 *
 * qt_wrapper.cpp — Enterprise C++ Implementation of Qt C API Bridge
 * =================================================================
 * Wraps Qt5 / Qt6 C++ classes into C-callable functions.
 */

#include "qtWrapper.h"

#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QDialog>
#include <QDoubleSpinBox>
#include <QPlainTextEdit>
#include <QRadioButton>
#include <QComboBox>
#include <QProgressBar>
#include <QSlider>
#include <QSpinBox>
#include <QResizeEvent>
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
#include <QStyle>
#include <QFile>
#include <QUrl>
#include <QIcon>

#include <QUiLoader>
#include <QMediaPlayer>
#include <QVideoWidget>

#include <QPixmap>
#include <QPainter>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QEvent>

#if __has_include(<QtCharts/QChartView>) || defined(QT_CHARTS_LIB)
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
using namespace QtCharts;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QT_CHARTS_USE_NAMESPACE
#endif
#endif

#include <cstdlib>
#include <cstring>
#include <string>

/* Thread-local string buffer to prevent concurrent/rapid getter buffer collisions */
static thread_local std::string tls_temp_str_buffer;

static const char* copy_qstring_to_temp(const QString& qstr) {
    tls_temp_str_buffer = qstr.toStdString();
    return tls_temp_str_buffer.c_str();
}

/* ============================================================
 * CanvasWidget — QWidget with double-buffered QPixmap backing
 * ============================================================ */
class CanvasWidget : public QWidget {
public:
    QPixmap* backing;
    QPainter* activePainter;

    CanvasWidget(QWidget* parent)
        : QWidget(parent), backing(nullptr), activePainter(nullptr)
    {
        backing = new QPixmap(200, 200);
        backing->fill(Qt::white);
        setMinimumSize(50, 50);
    }

    ~CanvasWidget() {
        delete activePainter;
        delete backing;
    }

    void resizeBacking(int w, int h) {
        if (w < 1) w = 1;
        if (h < 1) h = 1;
        QPixmap* nb = new QPixmap(w, h);
        nb->fill(Qt::white);
        QPainter p(nb);
        p.drawPixmap(0, 0, *backing);
        p.end();
        delete backing;
        backing = nb;
    }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter painter(this);
        painter.drawPixmap(0, 0, *backing);
    }

    void resizeEvent(QResizeEvent* event) override {
        resizeBacking(event->size().width(), event->size().height());
    }
};

extern "C" {

/* Generic Resource Cleanup Helper */
void qt_object_delete(void* handle) {
    if (!handle) return;
    QObject* obj = (QObject*)handle;
    if (!obj->parent()) {
        delete obj;
    }
}

class DropEventFilter : public QObject {
public:
    QtStringCallback callback;
    void* userdata;

    DropEventFilter(QtStringCallback cb, void* ud, QObject* parent = nullptr) 
        : QObject(parent), callback(cb), userdata(ud) {}

    bool eventFilter(QObject* watched, QEvent* event) override {
        if (event->type() == QEvent::DragEnter) {
            QDragEnterEvent* de = static_cast<QDragEnterEvent*>(event);
            if (de->mimeData()->hasUrls()) {
                de->acceptProposedAction();
                return true;
            }
        } else if (event->type() == QEvent::Drop) {
            QDropEvent* de = static_cast<QDropEvent*>(event);
            if (de->mimeData()->hasUrls()) {
                QString paths;
                for (const QUrl& url : de->mimeData()->urls()) {
                    if (!paths.isEmpty()) paths += "\n";
                    paths += url.toLocalFile();
                }
                if (callback) callback(paths.toUtf8().constData(), userdata);
                de->acceptProposedAction();
                return true;
            }
        }
        return QObject::eventFilter(watched, event);
    }
};

/* ============================================================
 * Dynamic UI Designer Loader (QtUiTools & QUiLoader)
 * ============================================================ */
QtWidgetHandle qt_uiloader_load_file(const char* filepath, QtWidgetHandle parent) {
    if (!filepath) return nullptr;
    QFile file(QString::fromUtf8(filepath));
    if (!file.open(QFile::ReadOnly)) {
        return nullptr;
    }
    QUiLoader loader;
    QWidget* widget = loader.load(&file, (QWidget*)parent);
    file.close();
    return (QtWidgetHandle)widget;
}

/* ============================================================
 * Multimedia Engine (QtMultimedia & QMediaPlayer)
 * ============================================================ */
QtMediaPlayerHandle qt_mediaplayer_create(void) {
    return (QtMediaPlayerHandle)new QMediaPlayer();
}

void qt_mediaplayer_set_media(QtMediaPlayerHandle player, const char* file_or_url) {
    if (player && file_or_url) {
        QUrl url = QUrl::fromUserInput(QString::fromUtf8(file_or_url));
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        ((QMediaPlayer*)player)->setSource(url);
#else
        ((QMediaPlayer*)player)->setMedia(QMediaContent(url));
#endif
    }
}

void qt_mediaplayer_play(QtMediaPlayerHandle player) {
    if (player) ((QMediaPlayer*)player)->play();
}

void qt_mediaplayer_pause(QtMediaPlayerHandle player) {
    if (player) ((QMediaPlayer*)player)->pause();
}

void qt_mediaplayer_stop(QtMediaPlayerHandle player) {
    if (player) ((QMediaPlayer*)player)->stop();
}

void qt_mediaplayer_set_volume(QtMediaPlayerHandle player, int volume) {
    if (player) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        ((QMediaPlayer*)player)->setVolume(volume);
#endif
    }
}

QtWidgetHandle qt_videowidget_create(QtWidgetHandle parent) {
    return (QtWidgetHandle)new QVideoWidget((QWidget*)parent);
}

void qt_mediaplayer_set_video_output(QtMediaPlayerHandle player, QtWidgetHandle video_widget) {
    if (player && video_widget) {
        ((QMediaPlayer*)player)->setVideoOutput((QVideoWidget*)video_widget);
    }
}

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

void qt_widget_set_accept_drops(QtWidgetHandle widget, bool accept) {
    if (widget) ((QWidget*)widget)->setAcceptDrops(accept);
}

void qt_widget_on_drop(QtWidgetHandle widget, QtStringCallback callback, void* user_data) {
    if (!widget || !callback) return;
    QObject* obj = (QObject*)widget;
    obj->installEventFilter(new DropEventFilter(callback, user_data, obj));
}

/* ============================================================
 * QMainWindow & Bars & Menus
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

QtMenuHandle qt_menu_add_submenu(QtMenuHandle menu, const char* title) {
    if (!menu || !title) return nullptr;
    QMenu* parentMenu = (QMenu*)menu;
    return (QtMenuHandle)parentMenu->addMenu(QString::fromUtf8(title));
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
 * Basic Controls & Signals
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

QtPixmapHandle qt_pixmap_create(int w, int h) {
    QPixmap* p = new QPixmap(w, h);
    p->fill(Qt::transparent);
    return (QtPixmapHandle)p;
}

void qt_pixmap_fill(QtPixmapHandle pixmap, const char* color_hex) {
    if (pixmap && color_hex) {
        ((QPixmap*)pixmap)->fill(QColor(QString::fromUtf8(color_hex)));
    }
}

void qt_pixmap_destroy(QtPixmapHandle pixmap) {
    if (pixmap) delete (QPixmap*)pixmap;
}

QtPainterHandle qt_painter_create(QtPixmapHandle pixmap) {
    if (!pixmap) return nullptr;
    QPainter* p = new QPainter((QPixmap*)pixmap);
    return (QtPainterHandle)p;
}

void qt_painter_set_pen(QtPainterHandle painter, const char* color_hex, int width) {
    if (painter && color_hex) {
        ((QPainter*)painter)->setPen(QPen(QColor(QString::fromUtf8(color_hex)), width));
    }
}

void qt_painter_set_brush(QtPainterHandle painter, const char* color_hex) {
    if (painter && color_hex) {
        ((QPainter*)painter)->setBrush(QBrush(QColor(QString::fromUtf8(color_hex))));
    }
}

void qt_painter_draw_line(QtPainterHandle painter, int x1, int y1, int x2, int y2) {
    if (painter) ((QPainter*)painter)->drawLine(x1, y1, x2, y2);
}

void qt_painter_draw_rect(QtPainterHandle painter, int x, int y, int w, int h) {
    if (painter) ((QPainter*)painter)->drawRect(x, y, w, h);
}

void qt_painter_draw_ellipse(QtPainterHandle painter, int x, int y, int w, int h) {
    if (painter) ((QPainter*)painter)->drawEllipse(x, y, w, h);
}

void qt_painter_draw_text(QtPainterHandle painter, int x, int y, const char* text) {
    if (painter && text) {
        ((QPainter*)painter)->drawText(x, y, QString::fromUtf8(text));
    }
}

void qt_painter_end(QtPainterHandle painter) {
    if (painter) {
        ((QPainter*)painter)->end();
        delete (QPainter*)painter;
    }
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
    if (QApplication::style()) {
        tray->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
    }
    if (tooltip) tray->setToolTip(QString::fromUtf8(tooltip));
    return (QtTrayHandle)tray;
}

void qt_tray_show(QtTrayHandle tray) {
    if (tray) ((QSystemTrayIcon*)tray)->show();
}

void qt_tray_hide(QtTrayHandle tray) {
    if (tray) ((QSystemTrayIcon*)tray)->hide();
}

void qt_tray_set_icon(QtTrayHandle tray, const char* icon_path) {
    if (!tray) return;
    QSystemTrayIcon* qtray = (QSystemTrayIcon*)tray;
    if (icon_path && strlen(icon_path) > 0) {
        qtray->setIcon(QIcon(QString::fromUtf8(icon_path)));
    }
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

/* ============================================================
 * Qt Charts
 * ============================================================ */
#if __has_include(<QtCharts/QChartView>) || defined(QT_CHARTS_LIB)
QtChartViewHandle qt_chartview_create(QtWidgetHandle parent) {
    QChartView* view = new QChartView((QWidget*)parent);
    view->setRenderHint(QPainter::Antialiasing);
    return (QtChartViewHandle)view;
}
void qt_chartview_set_chart(QtChartViewHandle view, QtChartHandle chart) {
    if (view && chart) ((QChartView*)view)->setChart((QChart*)chart);
}
QtChartHandle qt_chart_create(void) {
    return (QtChartHandle)new QChart();
}
void qt_chart_set_title(QtChartHandle chart, const char* title) {
    if (chart && title) ((QChart*)chart)->setTitle(QString::fromUtf8(title));
}
void qt_chart_add_series(QtChartHandle chart, QtSeriesHandle series) {
    if (chart && series) ((QChart*)chart)->addSeries((QAbstractSeries*)series);
}
void qt_chart_create_default_axes(QtChartHandle chart) {
    if (chart) ((QChart*)chart)->createDefaultAxes();
}
QtSeriesHandle qt_lineseries_create(void) {
    return (QtSeriesHandle)new QLineSeries();
}
void qt_lineseries_append(QtSeriesHandle series, double x, double y) {
    if (series) ((QLineSeries*)series)->append(x, y);
}
QtSeriesHandle qt_pieseries_create(void) {
    return (QtSeriesHandle)new QPieSeries();
}
void qt_pieseries_append(QtSeriesHandle series, const char* label, double value) {
    if (series && label) ((QPieSeries*)series)->append(QString::fromUtf8(label), value);
}

/* Bar Series */
QtSeriesHandle qt_barseries_create(void) {
    return (QtSeriesHandle)new QBarSeries();
}
QtSeriesHandle qt_barset_create(const char* label) {
    return (QtSeriesHandle)new QBarSet(QString::fromUtf8(label ? label : ""));
}
void qt_barset_append(QtSeriesHandle barset, double value) {
    if (barset) ((QBarSet*)barset)->append(value);
}
void qt_barseries_append(QtSeriesHandle series, QtSeriesHandle barset) {
    if (series && barset) ((QBarSeries*)series)->append((QBarSet*)barset);
}

/* Scatter Series */
QtSeriesHandle qt_scatterseries_create(void) {
    return (QtSeriesHandle)new QScatterSeries();
}
void qt_scatterseries_append(QtSeriesHandle series, double x, double y) {
    if (series) ((QScatterSeries*)series)->append(x, y);
}

/* Axis Customization */
void qt_chart_set_axis_title(QtChartHandle chart, int orientation, const char* title) {
    if (!chart || !title) return;
    Qt::Orientation orient = (orientation == 1) ? Qt::Vertical : Qt::Horizontal;
    QList<QAbstractAxis*> axesList = ((QChart*)chart)->axes(orient);
    for (QAbstractAxis* axis : axesList) {
        axis->setTitleText(QString::fromUtf8(title));
    }
}
void qt_chart_set_axis_range(QtChartHandle chart, int orientation, double min, double max) {
    if (!chart) return;
    Qt::Orientation orient = (orientation == 1) ? Qt::Vertical : Qt::Horizontal;
    QList<QAbstractAxis*> axesList = ((QChart*)chart)->axes(orient);
    for (QAbstractAxis* axis : axesList) {
        QValueAxis* valAxis = qobject_cast<QValueAxis*>(axis);
        if (valAxis) {
            valAxis->setRange(min, max);
        }
    }
}
#else
// Fallback if no charts available
QtChartViewHandle qt_chartview_create(QtWidgetHandle parent) { return nullptr; }
void qt_chartview_set_chart(QtChartViewHandle view, QtChartHandle chart) {}
QtChartHandle qt_chart_create(void) { return nullptr; }
void qt_chart_set_title(QtChartHandle chart, const char* title) {}
void qt_chart_add_series(QtChartHandle chart, QtSeriesHandle series) {}
void qt_chart_create_default_axes(QtChartHandle chart) {}
QtSeriesHandle qt_lineseries_create(void) { return nullptr; }
void qt_lineseries_append(QtSeriesHandle series, double x, double y) {}
QtSeriesHandle qt_pieseries_create(void) { return nullptr; }
void qt_pieseries_append(QtSeriesHandle series, const char* label, double value) {}
QtSeriesHandle qt_barseries_create(void) { return nullptr; }
QtSeriesHandle qt_barset_create(const char* label) { return nullptr; }
void qt_barset_append(QtSeriesHandle barset, double value) {}
void qt_barseries_append(QtSeriesHandle series, QtSeriesHandle barset) {}
QtSeriesHandle qt_scatterseries_create(void) { return nullptr; }
void qt_scatterseries_append(QtSeriesHandle series, double x, double y) {}
void qt_chart_set_axis_title(QtChartHandle chart, int orientation, const char* title) {}
void qt_chart_set_axis_range(QtChartHandle chart, int orientation, double min, double max) {}
#endif

/* ============================================================
 * Pixmap Save & Load
 * ============================================================ */
bool qt_pixmap_save(QtPixmapHandle pixmap, const char* path) {
    if (!pixmap || !path) return false;
    return ((QPixmap*)pixmap)->save(QString::fromUtf8(path));
}

QtPixmapHandle qt_pixmap_load(const char* path) {
    if (!path) return nullptr;
    QPixmap* p = new QPixmap(QString::fromUtf8(path));
    if (p->isNull()) {
        delete p;
        return nullptr;
    }
    return (QtPixmapHandle)p;
}

/* ============================================================
 * Painter — Extended Drawing Primitives
 * ============================================================ */
void qt_painter_draw_rounded_rect(QtPainterHandle painter, int x, int y, int w, int h, int radius) {
    if (painter) ((QPainter*)painter)->drawRoundedRect(x, y, w, h, radius, radius);
}

void qt_painter_draw_arc(QtPainterHandle painter, int x, int y, int w, int h, int startAngle, int spanAngle) {
    if (painter) ((QPainter*)painter)->drawArc(x, y, w, h, startAngle * 16, spanAngle * 16);
}

void qt_painter_draw_chord(QtPainterHandle painter, int x, int y, int w, int h, int startAngle, int spanAngle) {
    if (painter) ((QPainter*)painter)->drawChord(x, y, w, h, startAngle * 16, spanAngle * 16);
}

void qt_painter_draw_pie(QtPainterHandle painter, int x, int y, int w, int h, int startAngle, int spanAngle) {
    if (painter) ((QPainter*)painter)->drawPie(x, y, w, h, startAngle * 16, spanAngle * 16);
}

void qt_painter_draw_polygon(QtPainterHandle painter, const int* points, int count) {
    if (!painter || !points || count < 3) return;
    QPolygon poly;
    for (int i = 0; i < count; ++i) {
        poly << QPoint(points[i * 2], points[i * 2 + 1]);
    }
    ((QPainter*)painter)->drawPolygon(poly);
}

void qt_painter_draw_pixmap(QtPainterHandle painter, QtPixmapHandle pixmap, int x, int y) {
    if (painter && pixmap) ((QPainter*)painter)->drawPixmap(x, y, *(QPixmap*)pixmap);
}

/* Pen style: 0=Solid, 1=Dash, 2=Dot, 3=DashDot, 4=DashDotDot */
void qt_painter_set_pen_style(QtPainterHandle painter, int style, int width, const char* color_hex) {
    if (!painter) return;
    Qt::PenStyle ps = Qt::SolidLine;
    switch (style) {
        case 1: ps = Qt::DashLine; break;
        case 2: ps = Qt::DotLine; break;
        case 3: ps = Qt::DashDotLine; break;
        case 4: ps = Qt::DashDotDotLine; break;
        default: ps = Qt::SolidLine; break;
    }
    QPen pen(QColor(QString::fromUtf8(color_hex ? color_hex : "#000000")), width, ps);
    ((QPainter*)painter)->setPen(pen);
}

/* Font family, size, bold, italic */
void qt_painter_set_font(QtPainterHandle painter, const char* family, int size, bool bold, bool italic) {
    if (!painter) return;
    QFont font(QString::fromUtf8(family ? family : "Sans"), size);
    font.setBold(bold);
    font.setItalic(italic);
    ((QPainter*)painter)->setFont(font);
}

/* ============================================================
 * Canvas Widget — Double-buffered QPixmap-backed drawing area
 * ============================================================ */
QtWidgetHandle qt_canvas_create(QtWidgetHandle parent) {
    CanvasWidget* cw = new CanvasWidget((QWidget*)parent);
    return (QtWidgetHandle)cw;
}

QtPainterHandle qt_canvas_begin(QtWidgetHandle canvas) {
    if (!canvas) return nullptr;
    CanvasWidget* cw = (CanvasWidget*)canvas;
    if (cw->activePainter) {
        delete cw->activePainter;
    }
    cw->activePainter = new QPainter(cw->backing);
    return (QtPainterHandle)cw->activePainter;
}

void qt_canvas_end(QtWidgetHandle canvas) {
    if (!canvas) return;
    CanvasWidget* cw = (CanvasWidget*)canvas;
    if (cw->activePainter) {
        cw->activePainter->end();
        delete cw->activePainter;
        cw->activePainter = nullptr;
    }
    cw->update();
}

void qt_canvas_clear(QtWidgetHandle canvas, const char* color_hex) {
    if (!canvas) return;
    CanvasWidget* cw = (CanvasWidget*)canvas;
    cw->backing->fill(QColor(QString::fromUtf8(color_hex ? color_hex : "#ffffff")));
    cw->update();
}

void qt_canvas_set_size(QtWidgetHandle canvas, int w, int h) {
    if (!canvas) return;
    CanvasWidget* cw = (CanvasWidget*)canvas;
    cw->resizeBacking(w, h);
    cw->resize(w, h);
}

/* ============================================================
 * QDialog (Modal Dialog)
 * ============================================================ */
QtWidgetHandle qt_dialog_create(QtWidgetHandle parent) {
    QDialog* dlg = new QDialog((QWidget*)parent);
    dlg->setModal(true);
    dlg->setWindowTitle("Dialog");
    return (QtWidgetHandle)dlg;
}

int qt_dialog_exec(QtWidgetHandle dialog) {
    if (!dialog) return 0;
    return ((QDialog*)dialog)->exec();
}

/* ============================================================
 * QDoubleSpinBox (Decimal Spin Control)
 * ============================================================ */
QtWidgetHandle qt_doublespinbox_create(QtWidgetHandle parent) {
    QDoubleSpinBox* spin = new QDoubleSpinBox((QWidget*)parent);
    spin->setRange(0.0, 100.0);
    spin->setDecimals(2);
    spin->setSingleStep(0.1);
    return (QtWidgetHandle)spin;
}

void qt_doublespinbox_set_range(QtWidgetHandle spin, double min, double max) {
    if (spin) ((QDoubleSpinBox*)spin)->setRange(min, max);
}

void qt_doublespinbox_set_value(QtWidgetHandle spin, double value) {
    if (spin) ((QDoubleSpinBox*)spin)->setValue(value);
}

double qt_doublespinbox_get_value(QtWidgetHandle spin) {
    return spin ? ((QDoubleSpinBox*)spin)->value() : 0.0;
}

void qt_doublespinbox_set_decimals(QtWidgetHandle spin, int prec) {
    if (spin) ((QDoubleSpinBox*)spin)->setDecimals(prec);
}

/* ============================================================
 * QPlainTextEdit (Plain Text Editor)
 * ============================================================ */
QtWidgetHandle qt_plaintextedit_create(const char* text, QtWidgetHandle parent) {
    return (QtWidgetHandle)new QPlainTextEdit(QString::fromUtf8(text ? text : ""), (QWidget*)parent);
}

void qt_plaintextedit_append(QtWidgetHandle edit, const char* text) {
    if (edit && text) ((QPlainTextEdit*)edit)->appendPlainText(QString::fromUtf8(text));
}

void qt_plaintextedit_clear(QtWidgetHandle edit) {
    if (edit) ((QPlainTextEdit*)edit)->clear();
}

const char* qt_plaintextedit_get_text(QtWidgetHandle edit) {
    if (!edit) return "";
    return copy_qstring_to_temp(((QPlainTextEdit*)edit)->toPlainText());
}

void qt_plaintextedit_set_read_only(QtWidgetHandle edit, bool ro) {
    if (edit) ((QPlainTextEdit*)edit)->setReadOnly(ro);
}

} // extern "C"
