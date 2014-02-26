/*
 * Copyright (C) 2002 Robert Ernst <robert.ernst@maxxio.com>
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See COPYING for GPL licensing information.
 *
 */

#include "PlayField.h"

#include <QApplication>
#include <QResizeEvent>
#include <QFont>
#include <QFontMetrics>

#include "data.c"

PlayField::PlayField(QWidget *parent) :
    QWidget(parent/*, name, f*/)
{
    /* set up the default values for some member variables */
    m_timer_mode = TimerUp;
    m_map_mode = MapBuild;
    m_size = 9;
    m_xpos = -1;
    m_ypos = -1;
    m_dir = North;
    m_state = Intro;
    m_pixmap = 0;
    m_counter = 0;
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timerTick()));

    /* initialize the xpos/ypos increment counter */
    xpos_inc[North] = 0;
    xpos_inc[East] = 1;
    xpos_inc[South] = 0;
    xpos_inc[West] = -1;
    ypos_inc[North] = -1;
    ypos_inc[East] = 0;
    ypos_inc[South] = 1;
    ypos_inc[West] = 0;

    /* load the images from the data source */
    m_bkg_east.loadFromData(bkg_east_data_, sizeof (bkg_east_data_));
    m_bkg_north.loadFromData(bkg_north_data_, sizeof (bkg_north_data_));
    m_bkg_south.loadFromData(bkg_south_data_, sizeof (bkg_south_data_));
    m_bkg_west.loadFromData(bkg_west_data_, sizeof (bkg_west_data_));
    m_close_center.loadFromData(close_center_data_, sizeof (close_center_data_));
    m_close_center_bright.loadFromData(close_center_bright_data_, sizeof (close_center_bright_data_));
    m_close_left.loadFromData(close_left_data_, sizeof (close_left_data_));
    m_close_left_bright.loadFromData(close_left_bright_data_, sizeof (close_left_bright_data_));
    m_close_right.loadFromData(close_right_data_, sizeof (close_right_data_));
    m_close_right_bright.loadFromData(close_right_bright_data_, sizeof (close_right_bright_data_));
    m_cmp_east.loadFromData(cmp_east_data_, sizeof (cmp_east_data_));
    m_cmp_north.loadFromData(cmp_north_data_, sizeof (cmp_north_data_));
    m_cmp_south.loadFromData(cmp_south_data_, sizeof (cmp_south_data_));
    m_cmp_west.loadFromData(cmp_west_data_, sizeof (cmp_west_data_));
    m_far_center.loadFromData(far_center_data_, sizeof (far_center_data_));
    m_far_center_bright.loadFromData(far_center_bright_data_, sizeof (far_center_bright_data_));
    m_far_left.loadFromData(far_left_data_, sizeof (far_left_data_));
    m_far_left_bright.loadFromData(far_left_bright_data_, sizeof (far_left_bright_data_));
    m_far_right.loadFromData(far_right_data_, sizeof (far_right_data_));
    m_far_right_bright.loadFromData(far_right_bright_data_, sizeof (far_right_bright_data_));
    m_ground.loadFromData(ground_data_, sizeof (ground_data_));
    m_middle_center.loadFromData(middle_center_data_, sizeof (middle_center_data_));
    m_middle_center_bright.loadFromData(middle_center_bright_data_, sizeof (middle_center_bright_data_));
    m_middle_left.loadFromData(middle_left_data_, sizeof (middle_left_data_));
    m_middle_left_bright.loadFromData(middle_left_bright_data_, sizeof (middle_left_bright_data_));
    m_middle_right.loadFromData(middle_right_data_, sizeof (middle_right_data_));
    m_middle_right_bright.loadFromData(middle_right_bright_data_, sizeof (middle_right_bright_data_));
    m_numbers.loadFromData(numbers_data_, sizeof (numbers_data_));
    m_timeup.loadFromData(timeup_data_, sizeof (timeup_data_));
    m_title.loadFromData(title_data_, sizeof (title_data_));
    m_youwin.loadFromData(youwin_data_, sizeof (youwin_data_));
}

PlayField::~PlayField()
{
    delete m_pixmap;
    delete m_timer;
}

void PlayField::paintEvent(QPaintEvent *event)
{
    switch (m_state) {
    case Intro:
    drawIntro();
    break;
    case Playing:
    drawPlaying();
    break;
    case ViewMap:
    drawViewMap();
    break;
    case GameOverWin:
    drawGameOverWin();
    break;
    case GameOverLoose:
    drawGameOverLoose();
    break;
    }
}

void PlayField::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    setFocusPolicy(Qt::StrongFocus);
}

void PlayField::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_pixmap || event->size() != event->oldSize()) {
    if (m_pixmap) {
        delete m_pixmap;
    }
    m_pixmap = new QPixmap(event->size().width(), event->size().height());
    }
}

void PlayField::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
    switch (event->key()) {
    case Qt::Key_Up:
    if (m_state == Playing) {
        moveForward();
    }
    break;
    case Qt::Key_Down:
    if (m_state == Playing) {
        moveBackward();
    }
    break;
    case Qt::Key_Left:
    if (m_state == Playing) {
        turnLeft();
    }
    break;
    case Qt::Key_Right:
    if (m_state == Playing) {
        turnRight();
    }
    break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
    case Qt::Key_Return:
    if (m_state == Playing) {
        m_state = ViewMap;
        repaint();
    } else if (m_state == ViewMap) {
        m_state = Playing;
        repaint();
    } else {
        start();
    }
    break;
    case Qt::Key_Escape:
    if (m_state == Playing || m_state == ViewMap) {
        stop();
    }
    break;
    }
}

void PlayField::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    if (m_state == Playing) {
    int max_x = m_pixmap->width();
    if (event->y() > max_x) {
        /* click on the lower area of the screen: compass, maze, timer */
        m_state = ViewMap;
        repaint();
    } else if (abs(event->x() - event->y()) > 10
        && abs(max_x - event->x() - event->y()) > 10) {
        /* click in the upper/lower/left/right quarter of the screen */
        int x = event->x();
        int y = event->y();
        if (x > y) {
        if ((max_x - x) > y) {
            moveForward();
        } else {
            turnRight();
        }
        } else {
        if ((max_x - y) < x) {
            moveBackward();
        } else {
            turnLeft();
        }
        }

    }
    } else if (m_state == ViewMap) {
    m_state = Playing;
    repaint();
    } else {
    start();
    }
}

void PlayField::timerTick(void)
{
    if (m_timer_mode == TimerDown) {
    if (m_counter > 0) {
        m_counter--;
        if (m_counter == 0) {
        m_state = GameOverLoose;
        m_timer->stop();
        }
    }
    } else {
    m_counter++;
    }
    repaint();
}

void PlayField::start(void)
{
    m_state = Playing;
    m_timer->start(1000/*, false*/);
    m_counter = (m_timer_mode == TimerDown) ? m_size * m_size : 0;
    makeMaze();
    repaint();
}

void PlayField::stop(void)
{
    if (m_state != Intro) {
    m_state = Intro;
    m_timer->stop();
    repaint();
    }
}

void PlayField::updateTimerMode(int timer_mode)
{
    if (timer_mode != m_timer_mode) {
    switch (timer_mode) {
    case TimerDown:
        m_timer_mode = TimerDown;
        break;
    default:
        m_timer_mode = TimerUp;
        break;
    }
    if (m_state == Playing || m_state == ViewMap) {
        start();
    }
    }
}

void PlayField::updateMapMode(int map_mode)
{
    if (map_mode != m_map_mode) {
    switch (map_mode) {
    case MapAll:
        m_map_mode = MapAll;
        break;
    case MapNone:
        m_map_mode = MapNone;
        break;
    default:
        m_map_mode = MapBuild;
        break;
    }
    if (m_state == Playing || m_state == ViewMap) {
        start();
    }
    }
}

void PlayField::updateSize(int size)
{
    if (size != m_size) {
    m_size = size;
    if (m_state == Playing || m_state == ViewMap) {
        start();
    }
    }
}

void PlayField::drawIntro(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor black(0, 0, 0);
    QColor white(255, 255, 255);
    int max_x = width();
    int max_y = height();

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    QString copyright(tr("Original game by Bill Kendrick\n"
                         "Ported to the Zaurus by Robert Ernst\n"
                         "Ported to Qt and MotoMAGX by EXL"));
    QFont f1 = painter_pixmap.font();
    f1.setPixelSize(40);
    f1.setBold(true);
    QFont f2 = painter_pixmap.font();
    f2.setPixelSize(12);
    painter_pixmap.drawPixmap(0, 0, m_title);
    painter_pixmap.setFont(f1);
    painter_pixmap.setPen(black);
    painter_pixmap.drawText(5, 5, max_x, max_x / 2, Qt::AlignHCenter | Qt::AlignVCenter, tr("PDA Maze"));
    painter_pixmap.setPen(white);
    painter_pixmap.drawText(0, 0, max_x, max_x / 2, Qt::AlignHCenter | Qt::AlignVCenter, tr("PDA Maze"));
    painter_pixmap.setFont(f2);
    painter_pixmap.setPen(black);
    painter_pixmap.drawText(0, max_x, max_x, max_y - max_x, Qt::AlignHCenter | Qt::AlignVCenter, copyright);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::drawPlaying(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor white(255, 255, 255);
    int max_x = width();
    int max_y = height();

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    drawMazeView(painter_pixmap);
    drawCompass(painter_pixmap);
    drawTime(painter_pixmap);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::drawViewMap(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor black(0, 0, 0);
    QColor grey(192, 192, 192);
    QColor white(255, 255, 255);
    QColor red(255, 0, 0);
    int max_x = width();
    int max_y = height();
    int xsize = max_x / m_size;
    int ysize = max_x / m_size;
    int xoffs = (max_x - xsize * m_size) / 2;
    int yoffs = (max_x - ysize * m_size) / 2;
    int x;
    int y;

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    for (y = 0; y < m_size; y++) {
    for (x = 0; x < m_size; x++) {
        if (x == m_xpos && y == m_ypos) {
        painter_pixmap.fillRect(xoffs + x * xsize, yoffs + y * ysize, xsize, ysize, red);
        } else if (m_map_mode == MapNone
        || (m_map_mode == MapBuild && m_seen[y][x] == false)) {
        painter_pixmap.fillRect(xoffs + x * xsize, yoffs + y * ysize, xsize, ysize, grey);
        } else if (m_maze[y][x] == 255) {
        painter_pixmap.fillRect(xoffs + x * xsize, yoffs + y * ysize, xsize, ysize, black);
        } else {
        painter_pixmap.fillRect(xoffs + x * xsize, yoffs + y * ysize, xsize, ysize, white);
        }
    }
    }
    drawCompass(painter_pixmap);
    drawTime(painter_pixmap);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::drawGameOverWin(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor white(255, 255, 255);
    int max_x = width();
    int max_y = height();

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    int xoffs = (max_x - m_youwin.width()) / 2;
    int yoffs = (max_y - m_youwin.height()) / 2;
    drawMazeView(painter_pixmap);
    drawCompass(painter_pixmap);
    drawTime(painter_pixmap);
    painter_pixmap.drawPixmap(xoffs, yoffs, m_youwin);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::drawGameOverLoose(void)
{
    QPainter painter_pixmap(m_pixmap);
    QPainter painter_widget(this);
    QColor white(255, 255, 255);
    int max_x = width();
    int max_y = height();

    painter_pixmap.fillRect(0, 0, max_x, max_y, white);
    painter_pixmap.setPen(white);

    int xoffs = (max_x - m_timeup.width()) / 2;
    int yoffs = (max_y - m_timeup.height()) / 2;
    drawMazeView(painter_pixmap);
    drawCompass(painter_pixmap);
    drawTime(painter_pixmap);
    painter_pixmap.drawPixmap(xoffs, yoffs, m_timeup);

    //painter_pixmap.flush();
    painter_widget.drawPixmap(0, 0, *m_pixmap);
}

void PlayField::moveForward(void)
{
    int xpos = m_xpos + xpos_inc[m_dir];
    int ypos = m_ypos + ypos_inc[m_dir];

    if (m_maze[ypos][xpos] != 255) {
    m_xpos = xpos;
    m_ypos = ypos;
    m_seen[m_ypos - 1][m_xpos - 1] = true;
    m_seen[m_ypos - 1][m_xpos + 0] = true;
    m_seen[m_ypos - 1][m_xpos + 1] = true;
    m_seen[m_ypos + 0][m_xpos - 1] = true;
    m_seen[m_ypos + 0][m_xpos + 0] = true;
    m_seen[m_ypos + 0][m_xpos + 1] = true;
    m_seen[m_ypos + 1][m_xpos - 1] = true;
    m_seen[m_ypos + 1][m_xpos + 0] = true;
    m_seen[m_ypos + 1][m_xpos + 1] = true;
    if (m_xpos == 1 && m_ypos == 2) {
        m_state = GameOverWin;
        m_timer->stop();
    }
    repaint();
    }
}

void PlayField::moveBackward(void)
{
    int xpos = m_xpos - xpos_inc[m_dir];
    int ypos = m_ypos - ypos_inc[m_dir];

    if (m_maze[ypos][xpos] != 255) {
    m_xpos = xpos;
    m_ypos = ypos;
    m_seen[m_ypos - 1][m_xpos - 1] = true;
    m_seen[m_ypos - 1][m_xpos + 0] = true;
    m_seen[m_ypos - 1][m_xpos + 1] = true;
    m_seen[m_ypos + 0][m_xpos - 1] = true;
    m_seen[m_ypos + 0][m_xpos + 0] = true;
    m_seen[m_ypos + 0][m_xpos + 1] = true;
    m_seen[m_ypos + 1][m_xpos - 1] = true;
    m_seen[m_ypos + 1][m_xpos + 0] = true;
    m_seen[m_ypos + 1][m_xpos + 1] = true;
    if (m_xpos == 1 && m_ypos == 2) {
        m_state = GameOverWin;
    }
    repaint();
    }
}

void PlayField::turnLeft(void)
{
    switch (m_dir) {
    case North:
    m_dir = West;
    break;
    case East:
    m_dir = North;
    break;
    case South:
    m_dir = East;
    break;
    case West:
    m_dir = South;
    break;
    }
    repaint();
}

void PlayField::turnRight(void)
{
    switch (m_dir) {
    case North:
    m_dir = East;
    break;
    case East:
    m_dir = South;
    break;
    case South:
    m_dir = West;
    break;
    case West:
    m_dir = North;
    break;
    }
    repaint();
}

int PlayField::mazeChunk(int xpos, int ypos)
{
    /* This is the function mazechunk() from Bill Kendricks pdamaze.c */
    return (xpos > 0 && ypos > 0 && xpos < (m_size - 1) && ypos < (m_size - 1))
    ? m_maze[ypos][xpos] : -1;
}

void PlayField::makeMaze(void)
{
    /* This is the function create_maze() from Bill Kendricks pdamaze.c */
    int x;
    int y;

    for (y = 1; y < (m_size - 1); y++) {
    for (x = 1; x < (m_size - 1); x++) {
        m_maze[y][x] = 255;
    }
    m_maze[y][0] = -1;
    m_maze[y][m_size - 1] = -1;
    }
    for (x = 0; x < m_size; x++) {
    m_maze[0][x] = -1;
    m_maze[m_size - 1][x] = -1;
    }

    y = 2;
    x = 2;
    int xx = 0;
    int j = 0;
    int ok = 2;
    m_maze[y][x] = 5;

    do {
    if (ok == 2) {
        j = (rand() % 4);
        xx = j;
    }
    ok = 0;
    int nx = x + xpos_inc[j] * 2;
    int ny = y + ypos_inc[j] * 2;
    if (mazeChunk(nx, ny) == 255) {
        m_maze[ny][nx] = j + 1;
        m_maze[y + ypos_inc[j]][x + xpos_inc[j]] = 0;
        x = nx;
        y = ny;
        ok = 2;
    }
    if (ok == 0) {
        if (j < 3) {
        j = j + 1;
        while (j > 3) {
            j = j - 4;
        }
        } else {
        j = 0;
        }
        if (j != xx) {
        ok = 1;
        }
    }
    if (ok == 0) {
        j = mazeChunk(x, y);
        m_maze[y][x] = 0;
        if (j < 5) {
        x = x - (xpos_inc[j - 1] * 2);
        y = y - (ypos_inc[j - 1] * 2);
        ok = 2;
        }
    }
    } while (ok != 0);

    for (x = 0; x < m_size; x++) {
    m_seen[0][x] = 1;
    m_seen[1][x] = 1;
    m_seen[m_size - 2][x] = 1;
    m_seen[m_size - 1][x] = 1;
    }
    for (y = 2; y < m_size - 2; y++) {
    m_seen[y][0] = 1;
    m_seen[y][1] = 1;
    m_seen[y][m_size - 2] = 1;
    m_seen[y][m_size - 1] = 1;
    for (x = 2; x < m_size - 2; x++) {
        m_seen[y][x] = 0;
    }
    }

    m_maze[2][1] = 0;

    m_xpos = m_size / 2 + 1;
    m_ypos = m_size / 2 + 1;
    m_xpos = m_xpos + (m_xpos % 2);
    m_ypos = m_ypos + (m_ypos % 2);
    do {
    switch(rand() % 4) {
    case 0:
        m_dir = North;
        break;
    case 1:
        m_dir = South;
        break;
    case 2:
        m_dir = West;
        break;
    case 3:
        m_dir = East;
        break;
    }
    } while (m_maze[m_ypos + ypos_inc[m_dir]][m_xpos + xpos_inc[m_dir]] == 255);
}

void PlayField::drawMazeView(QPainter &painter)
{
    switch (m_dir) {
    case North:
    painter.drawPixmap(0, 0, m_bkg_north);
    break;
    case East:
    painter.drawPixmap(0, 0, m_bkg_east);
    break;
    case South:
    painter.drawPixmap(0, 0, m_bkg_south);
    break;
    case West:
    painter.drawPixmap(0, 0, m_bkg_west);
    break;
    }
    painter.drawPixmap(0, 120, m_ground);

    switch (m_dir) {
    case North:
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos - 3), DistFar, -1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos - 3), DistFar, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos - 3), DistFar, 0);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos - 2), DistMid, -1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos - 2), DistMid, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos - 2), DistMid, 0);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos - 1), DistClose, -1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos - 1), DistClose, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos - 1), DistClose, 0);
    break;
    case East:
    drawWall(painter, mazeChunk(m_xpos + 3, m_ypos - 1), DistFar, -1);
    drawWall(painter, mazeChunk(m_xpos + 3, m_ypos + 1), DistFar, 1);
    drawWall(painter, mazeChunk(m_xpos + 3, m_ypos + 0), DistFar, 0);
    drawWall(painter, mazeChunk(m_xpos + 2, m_ypos - 1), DistMid, -1);
    drawWall(painter, mazeChunk(m_xpos + 2, m_ypos + 1), DistMid, 1);
    drawWall(painter, mazeChunk(m_xpos + 2, m_ypos + 0), DistMid, 0);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos - 1), DistClose, -1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 1), DistClose, 1);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 0), DistClose, 0);
    break;
    case South:
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 3), DistFar, -1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 3), DistFar, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos + 3), DistFar, 0);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 2), DistMid, -1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 2), DistMid, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos + 2), DistMid, 0);
    drawWall(painter, mazeChunk(m_xpos + 1, m_ypos + 1), DistClose, -1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 1), DistClose, 1);
    drawWall(painter, mazeChunk(m_xpos + 0, m_ypos + 1), DistClose, 0);
    break;
    case West:
    drawWall(painter, mazeChunk(m_xpos - 3, m_ypos + 1), DistFar, -1);
    drawWall(painter, mazeChunk(m_xpos - 3, m_ypos - 1), DistFar, 1);
    drawWall(painter, mazeChunk(m_xpos - 3, m_ypos + 0), DistFar, 0);
    drawWall(painter, mazeChunk(m_xpos - 2, m_ypos + 1), DistMid, -1);
    drawWall(painter, mazeChunk(m_xpos - 2, m_ypos - 1), DistMid, 1);
    drawWall(painter, mazeChunk(m_xpos - 2, m_ypos + 0), DistMid, 0);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 1), DistClose, -1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos - 1), DistClose, 1);
    drawWall(painter, mazeChunk(m_xpos - 1, m_ypos + 0), DistClose, 0);
    break;
    }
}

void PlayField::drawCompass(QPainter &painter)
{
    int max_x = 240;

    switch (m_dir) {
    case North:
    painter.drawPixmap(0, max_x, m_cmp_north);
    break;
    case East:
    painter.drawPixmap(0, max_x, m_cmp_east);
    break;
    case South:
    painter.drawPixmap(0, max_x, m_cmp_south);
    break;
    case West:
    painter.drawPixmap(0, max_x, m_cmp_west);
    break;
    }
}

void PlayField::drawTime(QPainter &painter)
{
    int max_x = 240;

    int min = (m_counter / 60) % 60;
    int sec = m_counter % 60;
    int i;
    char str[5] = {
    (min / 10) + '0', (min % 10) + '0', ':', (sec / 10) + '0', (sec % 10) + '0'
    };
    for (i = 0; i < 5; i++) {
    int number = str[i] - '0';
    painter.drawPixmap(max_x - 60 + i * 12, max_x, m_numbers, number * 12, 0, 12, 18);
    }
}

void PlayField::drawWall(QPainter &painter, int block, enum Dist dist, int xoffset)
{
    if (block == 255) {
    if (dist == DistFar) {
        if (xoffset < 0) {
        drawFarCenter(painter, 120 - 18 + (xoffset * 36));
        drawFarLeft(painter, 120 - 18 + (xoffset * 36) + 36);
        } else if (xoffset == 0) {
        drawFarCenter(painter, 120 - 18);
        } else if (xoffset > 0) {
        drawFarCenter(painter, 120 - 18 + (xoffset * 36));
        drawFarRight(painter, 120 - 18 + (xoffset * 36) - 15);
        }
    } else if (dist == DistMid) {
        if (xoffset < 0) {
        drawMidCenter(painter, 120 - 49 - (99 - 36) + (xoffset * 36)); // 49 -> 49.5
        drawMidLeft(painter, 120 - 49 - (99 - 36) + (xoffset * 36) + 99);
        } else if (xoffset == 0) {
        drawMidCenter(painter, 120 - 49);
        } else if (xoffset > 0) {
        drawMidCenter(painter, 120 - 49 + (99 - 36) + (xoffset * 36));
        drawMidRight(painter, 120 - 49 + (99 - 36) + (xoffset * 36) - 31); // 30 -> 30.5
        }
    } else if (dist == DistClose) {
        if (xoffset < 0) {
        drawCloseLeft(painter);
        } else if (xoffset == 0) {
        drawCloseCenter(painter);
        } else if (xoffset > 0) {
        drawCloseRight(painter);
        }
    }
    }
}

void PlayField::drawFarCenter(QPainter &painter, int xx)
{
    painter.drawPixmap(xx, 120 - 18,
    (m_dir == North || m_dir == West) ? m_far_center_bright : m_far_center);
}

void PlayField::drawFarLeft(QPainter &painter, int xx)
{
    QPixmap pix;
    int y;

    pix = (m_dir == North || m_dir == East) ? m_far_left_bright : m_far_left;
    for (y = 0; y < 16; y++) { // 16.5 -> 16
    painter.drawPixmap(xx, 120 - 18 + y, pix, 0, y, y, 1);
    painter.drawPixmap(xx, 120 - 18 + 34 - y, pix, 0, 34 - y, y, 1); // 34.5 -> 34
    }
    painter.drawPixmap(xx, 120 - 18 + 16, pix, 0, 16, 18, 3); // 16.5 -> 16
}

void PlayField::drawFarRight(QPainter &painter, int xx)
{
    QPixmap pix;
    int y;

    pix = (m_dir == South || m_dir == West) ? m_far_right_bright : m_far_right;
    for (y = 0; y < 17; y++) { // 16.5 -> 16
    painter.drawPixmap(xx + 17 - y, 120 - 18 + y, pix, 17 - y, y, y, 1); // 16.5 -> 16
    painter.drawPixmap(xx + 17 - y, 120 - 18 + 36 - y, pix, 17 - y, 36 - y, y, 1); // 16.5 -> 16
    }
    painter.drawPixmap(xx, 120 - 18 + 16, pix, 0, 16, 18, 4); // 16.5 -> 16
}

void PlayField::drawMidCenter(QPainter &painter, int xx)
{
    painter.drawPixmap(xx, 120 - 49,
    (m_dir == North || m_dir == West) ? m_middle_center_bright : m_middle_center); // 49 -> 49.5
}

void PlayField::drawMidLeft(QPainter &painter, int xx)
{
    QPixmap pix;
    int y;

    pix = (m_dir == North || m_dir == East) ? m_middle_left_bright : m_middle_left;
    for (y = 0; y < 33; y++) {
    painter.drawPixmap(xx, 120 - 49 + y, pix, 0, y, y, 1); // 49 -> 49.5
    painter.drawPixmap(xx, 120 - 49 + 99 - y, pix, 0, 99 - y, y, 1); // 49 -> 49.5
    }
    painter.drawPixmap(xx, 120 - 49 + 33, pix, 0, 33, 36, 37); // 49 -> 49.5, 37 -> 37.5
}

void PlayField::drawMidRight(QPainter &painter, int xx)
{
    QPixmap pix;
    int y;

    pix = (m_dir == South || m_dir == West) ? m_middle_right_bright : m_middle_right;
    for (y = 0; y < 34; y++) {
    painter.drawPixmap(xx + 32 - y, 120 - 49 + y, pix, 34 - y, y, y, 1); // 49 -> 49.5
    painter.drawPixmap(xx + 32 - y, 120 - 49 + 99 - y, pix, 34 - y, 99 - y, y, 1); // 49 -> 49.5
    }
    painter.drawPixmap(xx, 120 - 49 + 32, pix, 0, 33, 36, 38); // 49 -> 49.5, 37 -> 37.5
}

void PlayField::drawCloseCenter(QPainter &painter)
{
    painter.drawPixmap(0, 0,
    (m_dir == North || m_dir == West) ? m_close_center_bright : m_close_center);
}

void PlayField::drawCloseLeft(QPainter &painter)
{
    QPixmap pix;
    int y;

    pix = (m_dir == North || m_dir == East) ? m_close_left_bright : m_close_left;
    for (y = 0; y < 76; y++) { // 76 -> 76.5
    painter.drawPixmap(0, y, pix, 0, y, y, 1);
    painter.drawPixmap(0, 240 - y, pix, 0, 240 - y, y, 1);
    }
    painter.drawPixmap(0, 76, pix, 0, 76, 75, 90); // 76 -> 76.5
}

void PlayField::drawCloseRight(QPainter &painter)
{
    QPixmap pix;
    int y;

    pix = (m_dir == South || m_dir == West) ? m_close_right_bright : m_close_right;
    for (y = 0; y < 76; y++) { // 76 -> 76.5
    painter.drawPixmap(240 - y, y, pix, 76 - y, y, y, 1); // 76 -> 76.5
    painter.drawPixmap(240 - y, 240 - y, pix, 76 - y, 240 - y, y, 1); // 76 -> 76.5
    }
    painter.drawPixmap(165, 76, pix, 0, 76, 75, 90); // 76 -> 76.5
}
