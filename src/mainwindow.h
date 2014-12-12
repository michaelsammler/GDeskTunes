#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebView>
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
#include <QWinJumpList>
#endif

// Forward declarations
namespace Ui {
class GDeskTunes;
}

/*
 * This class is the implementation of the GUI for all platforms.
 * GDeskTunes specific functionality is implemented in the GDeskTunes subclass.
 *
 * Responsibilities of this class are:
 * - Platform dependent UI
 * - Platform dependent functionality (ThumbnailToolBar, JumpList, ...)
 * - Platform dependent behaviour (Full Screen, Maximize, ...)
 * - Media Keys
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void play();
    void next();
    void previous();
    void changeSettings();

public slots:
    virtual void switchMini() = 0;

    bool eventFilter(QObject *sender, QEvent *event);

    void switchMenu();
    void switchFullScreen();
    void setMenuVisible(bool visible);

    void about();
    void show();

    void receiveMacMediaKey(int key, bool repeat, bool pressed);

    void quitGDeskTunes();
    void closeWindow();

    void shuffle(QString mode);
    void repeat(QString mode);

    void isPlaying(bool playing);
    void activateWindow();
    void zoom();

public:
    virtual bool isMini();
    void keyPressEvent(QKeyEvent *event);

protected:
    void keyReleaseEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);

#if defined Q_OS_WIN && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);
#endif

    void updateJumpList();

private:
    void createThumbnailToolBar();
    void createJumpList();

    void setupActions();
    void checkAction(QKeyEvent *event, QAction *action);

public:
    Ui::GDeskTunes *ui;

    // UI stuff that can not be created with Qt Creator
    QMenu *shuffle_menu;
    QAction *shuffle_on;
    QAction *shuffle_off;

    QMenu *repeat_menu;
    QAction *repeat_off;
    QAction *repeat_all;
    QAction *repeat_one;

    // Windows specific UI stuff
#if defined(Q_OS_WIN) && QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    QWinJumpList* jumplist;
    QWinJumpListItem *change_style;
    QWinJumpListItem* show_hide_menu;
    QWinJumpListItem* fullscreen_menu;
    QWinJumpListItem* switchmini_menu;
    QWinJumpListItem* about_menu;
#endif

    void populateJumpList();
protected:
    QPoint windows_offset; // This variable can be used to position a frameless windows correctly
    bool draggable;        // Must be set when the window becomes draggable

    // Properties that control the behaviour of the application
    bool hide_menu;
private:
    // Set when quitting the application
    bool quitting;

    // Variables to drag the mini player without title bar
    bool do_move;
    int mouse_click_x_coordinate;
    int mouse_click_y_coordinate;
};

#endif // MAINWINDOW_H