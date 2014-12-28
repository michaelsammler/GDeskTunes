#define QT_NO_DEBUG_OUTPUT

#include "miniplayer.h"
#include "ui_miniplayer.h"

#include "mainwindow.h"

#include <QKeyEvent>
#include <QDebug>
#include <QScreen>


QString seconds_to_DHMS(quint32 duration)
{
    QString res;
    int seconds = (int) (duration % 60);
    duration /= 60;
    int minutes = (int) (duration % 60);
    duration /= 60;
    int hours = (int) (duration % 24);
    int days = (int) (duration / 24);
    if((hours == 0)&&(days == 0))
        return res.sprintf("%2d:%02d", minutes, seconds);
    if (days == 0)
        return res.sprintf("%2d:%02d:%02d", hours, minutes, seconds);
    return res.sprintf("%dd%02d:%02d:%02d", days, hours, minutes, seconds);
}

MiniPlayer::MiniPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MiniPlayer),
    trayIconTiming(0),
    trayIconPosition(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()),
    userIconTiming(std::numeric_limits<qint64>::max()),
    userPosition(std::numeric_limits<int>::max(), std::numeric_limits<int>::max()),
    is_tray(false),
    inactiveTiming(0),
    large(false),
    do_move(false),
    has_moved(false),
    inverted(false),
    gray(255),
    style(""),
    corner(0),
    slider_moving(false)
{
    ui->setupUi(this);

    Qt::WindowFlags flags;
    flags = Qt::Window;
    flags |= Qt::FramelessWindowHint;

    setWindowFlags(flags);

    setAttribute(Qt::WA_Hover);
}

MiniPlayer::~MiniPlayer()
{
    delete ui;
}

QScreen* getScreen(QPoint &pt)
{
    QCoreApplication *app = QCoreApplication::instance();

    QApplication *gui = dynamic_cast<QApplication*>(app);
    if ( gui == 0)
        return 0;

    QList<QScreen *> screens = gui->screens();
    for(int s=0; s<screens.size(); ++s)
    {
        QRect screen_rect = screens.at(s)->geometry();
        if (screen_rect.contains(pt))
            return screens.at(s);
    }

    return 0;
}

void MiniPlayer::placeMiniPlayer(QPoint& pt)
{
    qDebug() << "MiniPlayer::placeMiniPlayer(" << pt << ")";

    int x = pt.x();
    int y = pt.y();
    QScreen *screen = getScreen(pt);
    if (screen == 0)
    {
        if (pt.y() > 100)
            y -= height();
    }
    else
    {
        x = qMin(x, screen->availableGeometry().topRight().x() - width());
        y = qMin(y, screen->availableGeometry().bottomLeft().y() - height());
    }

    trayIconPosition.setX(x);
    trayIconPosition.setY(y);
    trayIconTiming = QDateTime::currentMSecsSinceEpoch();

    qDebug() << inactiveTiming << trayIconTiming << (inactiveTiming + 250 > trayIconTiming);
    if (inactiveTiming + 250 > trayIconTiming)
        return;

    if (isVisible())
        hide();
    else
    {
        show();
        raise();
        activateWindow();
    }
}

void MiniPlayer::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case Qt::Key_MediaPlay:
    case Qt::Key_MediaNext:
    case Qt::Key_MediaPrevious:
    {
        MainWindow *w = qobject_cast<MainWindow*>(parent());
        if (w != 0)
        {
            qDebug() << "MiniPlayer captures media key";
            // w->keyPressEvent(event);
            emit keyPressed(event);
            break;
        }
    }
    default:
        qDebug() << "Key Pressed" << event;
        QWidget::keyPressEvent(event);
        if (!event->isAccepted())
        {
            qDebug() << "emit keyPressed(" << event << ")";
            emit keyPressed(new QKeyEvent(event->type(), event->key(), event->modifiers()));
            event->ignore();
        }
        break;
    }
}

void MiniPlayer::nowPlaying(QString title, QString artist, QString album, int duration)
{
    qDebug() << "MiniPlayer::nowPlaying(" << title << "," << artist << "," << album << "," << duration << ")";
    ui->title->setText(title);
    ui->artist->setText(artist);
    ui->album->setText(album);
    ui->duration->setText(seconds_to_DHMS(duration) );
    ui->slider->setMinimum(0);
    ui->slider->setMaximum(duration);
}

void MiniPlayer::playbackTime(int current, int)
{
    if (!slider_moving)
    {
        // ui->current->setText( seconds_to_DHMS(current / 1000));
        ui->slider->setValue(current / 1000);
    }
}

void MiniPlayer::on_slider_valueChanged(int s)
{
    ui->current->setText( seconds_to_DHMS(s));
}


void MiniPlayer::albumArt(QPixmap pixmap)
{
    int w = ui->album_art->width();
    int h = ui->album_art->height();

    album_picture = pixmap;
    ui->album_art->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio, Qt::SmoothTransformation));

    if (large)
    {
        enableBackground();
    }
}

void MiniPlayer::changeBackground()
{
    large = !large;

    if (large)
        enableBackground();
    else
    {
        disableBackground();
    }
}

QScreen* getBestScreen(QWidget *widget)
{
    QCoreApplication *app = QCoreApplication::instance();

    QApplication *gui = dynamic_cast<QApplication*>(app);
    if ( gui == 0)
        return 0;

    QScreen *best_screen = 0;
    int largest_overlap = 0;
    QRect rect = widget->geometry();

    QList<QScreen *> screens = gui->screens();
    for(int s=0; s<screens.size(); ++s)
    {
        QRect screen_rect = screens.at(s)->geometry();

        QRect overlap = screen_rect.intersected(rect);
        int area = overlap.width() * overlap.height();
        if (area > largest_overlap)
        {
            largest_overlap = area;
            best_screen = screens.at(s);
        }
    }

    return best_screen;
}

void MiniPlayer::changeEvent(QEvent *event)
{
    qDebug() << "MiniPlayer::changeEvent(" << event->type()  << ")";

    switch(event->type()) {
    case QEvent::WindowStateChange:
    {
        if (this->windowState() & Qt::WindowMaximized)
        {
            qDebug() << "emit MiniPlayer::stateMaximized()";
            emit stateMaximized();
        }
        if (this->windowState() == Qt::WindowNoState)
        {
            qDebug() << "emit MiniPlayer::stateNone()";
            emit stateNone();
        }
        if (this->windowState() & Qt::WindowActive)
        {
            qDebug() << "emit MiniPlayer::stateActive()";
            emit stateActivated();
        }
         if (this->windowState() & Qt::WindowMinimized)
         {
            qDebug() << "emit MiniPlayer::stateMinimized()";
            emit stateMinimized();
         }
         if (windowState() & Qt::WindowFullScreen)
         {
             qDebug() << "emit MiniPlayer::stateFullscreen()";
             emit stateFullscreen();
         }
         break;
    }
    case QEvent::ActivationChange:
        if (isActiveWindow())
        {
            qDebug() << "emit MiniPlayer::windowActivated()";
            emit windowActivated();
        }
        else
        {
#ifdef Q_OS_MAC
            if (is_tray)
            {
                // Tray has changed mode
                hide();
            }
#endif
            qDebug() << "emit MiniPlayer::windowDeactivated()";
            emit windowDeactivated();
        }
        break;
    default:
        break;
    }
    QMainWindow::changeEvent(event);

}

void MiniPlayer::determineCorner(QScreen *screen)
{
    corner = 1;

    QRect screen_rect = screen->geometry();
    QSize half_size = screen->size() / 2;
    QPoint position = pos();

    QRect rect1(screen_rect.topLeft(), half_size);
    if (rect1.contains(position))
    {
        corner = 1;
        return;
    }
    QRect rect2(rect1.topRight(), half_size);
    if (rect2.contains(position))
    {
        corner = 2;
        return;
    }
    QRect rect4(rect1.bottomLeft(), half_size);
    if (rect4.contains(position))
    {
        corner = 4;
        return;
    }
    corner = 3;
}

void MiniPlayer::resize(int w, int h)
{
    QPoint topleft = pos();
    QSize current_size = size();
    QMainWindow::resize(w, h);
    switch(corner)
    {
    case 2:
        QMainWindow::resize(w, h);
        move(topleft.x() + current_size.width() - w, topleft.y());
        break;
    case 3:
        move(topleft.x() + current_size.width() - w, topleft.y() + current_size.height() - h);
        QMainWindow::resize(w, h);
        break;
    case 4:
        move(topleft.x(), topleft.y() + current_size.height() - h);
        break;
    case 1:
    default:
        // Nothing to do
        break;
    }
}

void MiniPlayer::enableBackground()
{
    qDebug() << "MiniPlayer::enableBackground()";
    if (album_picture.width() == 0) return;
    ui->album_art->setLarge(true);
    QPalette palette;
    int w = width();

    // Adjust height according to the scale factor for the width;
    float factor = w / (float)album_picture.width() ;
    int new_h = (int)(album_picture.height() * factor);

    if (corner == 0)
    {
        QScreen *screen = getBestScreen(this);
        determineCorner(screen);
    }

    qDebug() << w << "  " << new_h << " " << factor;
    resize(w, new_h);

    QImage image(w, new_h, QImage::Format_ARGB32);
    QPainter painter;
    painter.begin(&image);
    painter.fillRect(image.rect(), QColor(0, 0, 0, 255));
    painter.setOpacity(0.75);  //0.00 = 0%, 1.00 = 100% opacity.
    painter.drawPixmap(0, 0, album_picture.scaled(w, new_h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    painter.end();
    palette.setBrush(QPalette::Background, QBrush(image));
    setPalette(palette);

    int size = image.width() * image.height();
    if (size != 0)
    {
        double global_value = 0;
        for(int r=0; r<image.height(); ++r)
        {
            for(int c=0; c<image.width(); ++c)
            {
                global_value += qGray(image.pixel(c, r));
            }
        }
        global_value /= size;
        invert(global_value < 128);
    }

    ui->centralwidget->setStyleSheet("");
}

void MiniPlayer::disableBackground()
{
    ui->album_art->setLarge(false);
    QPalette palette;
    setPalette(palette);

    if (corner == 0)
        determineCorner(getBestScreen(this));

    resize(336, 130);

    ui->centralwidget->setStyleSheet(style);

    invert(gray < 128);
}

void MiniPlayer::rating(int rate)
{
    this->_rating = rate;
    // qDebug() << "MiniPlayer::rating(" << rate << ")";
    switch(rate)
    {
    case 1:
    case 2:
        ui->toggle_thumbs_down->setIcon(QIcon(":/icons/32x32/thumbs_down_on.png"));
        setIcon(ui->toggle_thumbs_up, ":/icons/32x32/thumbs_up_off");
    break;
    case 4:
    case 5:
        setIcon(ui->toggle_thumbs_down, ":/icons/32x32/thumbs_down_off");
        ui->toggle_thumbs_up->setIcon(QIcon(":/icons/32x32/thumbs_up.png"));
        break;
    default:
        setIcon(ui->toggle_thumbs_down, ":/icons/32x32/thumbs_down_off");
        setIcon(ui->toggle_thumbs_up, ":/icons/32x32/thumbs_up_off");
    }
}

void MiniPlayer::repeat(QString mode)
{
    this->_repeat = mode;

    if (mode == "LIST_REPEAT")
    {
        ui->toggle_repeat->setIcon(QIcon(":/icons/32x32/repeat_on.png"));
    }
    if (mode == "SINGLE_REPEAT")
    {
        ui->toggle_repeat->setIcon(QIcon(":/icons/32x32/repeat_one.png"));
    }
    if (mode == "NO_REPEAT")
    {
        setIcon(ui->toggle_repeat, ":/icons/32x32/repeat_off");
    }
}

void MiniPlayer::shuffle(QString mode)
{
    this->_shuffle = mode;

    if (mode == "ALL_SHUFFLE")
    {
        ui->toggle_shuffle->setIcon(QIcon(":/icons/32x32/shuffle_on"));
    }
    if (mode == "NO_SHUFFLE")
    {
        setIcon(ui->toggle_shuffle, ":/icons/32x32/shuffle_off");
    }
}

void MiniPlayer::mousePressEvent(QMouseEvent *event)
{
    mouse_click_x_coordinate = event->x();
    mouse_click_y_coordinate = event->y();
    do_move = true;
    has_moved = false;
    event->accept();
}

void MiniPlayer::mouseMoveEvent(QMouseEvent *event)
{
    if (do_move)
    {
        int dx = event->globalX() - mouse_click_x_coordinate;
        int dy = event->globalY() - mouse_click_y_coordinate;
        this->move(dx, dy);
        event->accept();
        if (dx > 2 || dy > 2)
            has_moved = true;
    }
    else
    {
        QWidget::mouseMoveEvent(event);
    }
}

void MiniPlayer::mouseReleaseEvent(QMouseEvent *event)
{
    do_move = false;
    userPosition.setX(event->globalX() - mouse_click_x_coordinate);
    userPosition.setY(event->globalY() - mouse_click_y_coordinate);
    userIconTiming = QDateTime::currentMSecsSinceEpoch();
    QWidget::mouseReleaseEvent(event);

    if (has_moved)
    {
        is_tray = false;
        has_moved = false;
        corner = 0;
        emit moved();
    }
}

void MiniPlayer::showEvent(QShowEvent *event)
{
    qDebug() << "emit MiniPlayer::shown()";
    emit shown();
    QMainWindow::showEvent(event);
}

void MiniPlayer::hideEvent(QHideEvent *event)
{
    qDebug() << "emit MiniPlayer::hidden()";
    emit hidden();
    QMainWindow::hideEvent(event);
}


void MiniPlayer::activateWindow()
{
    qDebug() << "MiniPlayer::activateWindow()";
    return QMainWindow::activateWindow();
}

void MiniPlayer::hide()
{
    qDebug() << "MiniPlayer::hide()";
    QMainWindow::hide();
}

void MiniPlayer::hideTray()
{
    qDebug() << "MiniPlayer::hideTray()";
    if (is_tray)
    {
        inactiveTiming = QDateTime::currentMSecsSinceEpoch();
        hide();
    }
}

void MiniPlayer::raise()
{
    qDebug() << "MiniPlayer::raise()";
    QMainWindow::raise();
}

void MiniPlayer::show()
{
    qDebug() << "MiniPlayer::show()";
    // Check to which position we have to move the player
    qint64 current = QDateTime::currentMSecsSinceEpoch();
    qDebug() << current << " " << trayIconTiming << " " << userIconTiming;
    bool savePosition = false;
    if (current < trayIconTiming + 100)
    {
        // Place it on the trayIcon position
        move(trayIconPosition.x(), trayIconPosition.y());
        is_tray = true;
    }
    else if (current > userIconTiming)
    {
        move(userPosition.x(), userPosition.y());
        is_tray = false;
    }
    else
        savePosition = true;
    QMainWindow::show();
    if (savePosition)
    {
        userPosition = pos();
        userIconTiming = current;
    }
}

#ifdef Q_OS_WIN
#include <Windows.h>
#include <WinUser.h>
#endif


void MiniPlayer::bringToFront()
{
#ifdef Q_OS_WIN
    // ::SetWindowPos((HWND)effectiveWinId(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    // ::SetWindowPos((HWND)effectiveWinId(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
#endif

    Qt::WindowFlags flags = windowFlags();
    if (on_top)
        flags |= Qt::WindowStaysOnTopHint;
    else
        flags &= ~Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);

    show();
    raise();
    activateWindow();
}

void MiniPlayer::setBackgroundColor(QColor color)
{
    qDebug() << "MiniPlayer::setBackgroundColor(" << color << ")";
    style = QString("background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(%1, %2, %3, 255), stop:1 rgba(%4, %5, %6, 255))");

    gray = qGray(color.rgb());

    int h,s,v;
    color.getHsv(&h, &s, &v);

    QColor second_color = QColor::fromHsv(h, s, v + (gray < 128 ? 20 : -20));
    style = style.arg(second_color.red()).arg(second_color.green()).arg(second_color.blue()).arg(color.red()).arg(color.green()).arg(color.blue());
    qDebug() << style;

    ui->centralwidget->setStyleSheet(style);

    invert(gray < 128);
}

void MiniPlayer::invert(bool inv)
{
    qDebug() << "MiniPlayer::invert(" << inv << ") inverted: " << inverted;
    if (inverted == inv) return;
    inverted = inv;

    setIcon(ui->play, ":/icons/32x32/play");
    setIcon(ui->previous, ":/icons/32x32/prev");
    setIcon(ui->next, ":/icons/32x32/next");
    setIcon(ui->closeMini, ":/icons/8x8/close_delete");
    repeat(this->_repeat);
    shuffle(this->_shuffle);
    rating(this->_rating);

    QString text = "color: ";
    if (inv)
        text += "white";
    else
        text += "black";
    ui->album->setStyleSheet(text);
    ui->artist->setStyleSheet(text);
    ui->title->setStyleSheet(text);
}

void MiniPlayer::setIcon(QAbstractButton* button, QString base)
{
    // qDebug() << "MiniPlayer::setIcon( " << button->objectName() << ") inverted: " << inverted;
    if (inverted)
    {
        base += "_dark.png";
    }
    else
        base += ".png";
    button->setIcon(QIcon(base));
}

void MiniPlayer::isPlaying(int mode)
{
    ui->play->setEnabled(mode != 0);
    if (mode == 0)
    {
        setIcon(ui->play, ":/icons/32x32/play");
    }
    if (mode == 1)
    {
        setIcon(ui->play, ":/icons/32x32/play");
    }
    if (mode == 2)
    {
        setIcon(ui->play, ":/icons/32x32/pause");
    }
}

void MiniPlayer::rewindEnabled(int mode)
{
    ui->previous->setEnabled(mode != 0);
}

void MiniPlayer::forwardEnabled(int mode)
{
    ui->next->setEnabled(mode != 0);
}

void MiniPlayer::setMiniPlayerOnTop(bool top)
{
    qDebug() << "MiniPlayer::setMiniPlayerOnTop(" << top << ")";

    Qt::WindowFlags flags = windowFlags();
    if (top)
        flags |= Qt::WindowStaysOnTopHint;
    else
        flags &= ~Qt::WindowStaysOnTopHint;
    setWindowFlags(flags);

    this->on_top = top;

    if (this->isVisible())
    {
        show();
    }
}

void MiniPlayer::on_slider_sliderPressed()
{
    slider_moving = true;
}

void MiniPlayer::on_slider_sliderReleased()
{
    qDebug() << "MiniPlayer::on_slider_sliderReleased()";
    int value = ui->slider->value();
    emit changePlaybackTime(value * 1000);
    slider_moving = false;
}

bool MiniPlayer::event(QEvent *event)
{
    if (event->type() == QEvent::HoverLeave)
    {
        showElements(false | !large);
    }
    if (event->type() == QEvent::HoverEnter)
    {
        showElements(true);
    }
    return QMainWindow::event(event);
}

void MiniPlayer::showElements(bool visible)
{
    ui->control_frame->setVisible(visible);
    ui->top_row->setVisible(visible);
}
